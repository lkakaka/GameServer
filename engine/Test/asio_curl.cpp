/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 2012, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

 /*
  * file: asiohiper.cpp
  * Example program to demonstrate the use of multi socket interface
  * with boost::asio
  *
  * This program is in c++ and uses boost::asio instead of libevent/libev.
  * Requires boost::asio, boost::bind and boost::system
  *
  * This is an adaptation of libcurl's "hiperfifo.c" and "evhiperfifo.c"
  * sample programs. This example implements a subset of the functionality from
  * hiperfifo.c, for full functionality refer hiperfifo.c or evhiperfifo.c
  *
  * Written by Lijo Antony based on hiperfifo.c by Jeff Pohlmeyer
  *
  * When running, the program creates an easy handle for a URL and
  * uses the curl_multi API to fetch it.
  *
  * Note:
  *  For the sake of simplicity, URL is hard coded to "www.google.com"
  *
  * This is purely a demo app, all retrieved data is simply discarded by the write
  * callback.
  */

// https://curl.se/libcurl/c/asiohiper.html

#include <string.h>
#include <iostream>

#include <curl/curl.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Test.h"


#define MSG_OUT stdout  

  /* boost::asio related objects
   * using global variables for simplicity
   */
boost::asio::io_service io_service;
boost::asio::deadline_timer timer(io_service);

typedef struct _SocketInfo
{
    _SocketInfo(boost::asio::io_service& io)
        : sock(io),
        mask(0)
    {

    }

    boost::asio::ip::tcp::socket sock;
    int mask; // is used to store current action 
} SocketInfo;
typedef boost::shared_ptr<SocketInfo> socket_ptr;

// windows 上连接超时， eventcb 会一直挂起。需要在 closesocket 回调中取消所有异步事件
// 取消异步事件又会导致， eventcb 在 closesocket 回调 之后被调用。
// 所以不能简单的在 closesocket 回调 中释放 socket_ptr 的资源。所以这里得用 shared_ptr
std::map<curl_socket_t, socket_ptr> socket_map;

/* Global information, common to all connections */
typedef struct _GlobalInfo
{
    CURLM* multi;
    int still_running;
} GlobalInfo;

/* Information associated with a specific easy handle */
typedef struct _ConnInfo
{
    CURL* easy;
    char* url;
    GlobalInfo* global;
    char error[CURL_ERROR_SIZE];
} ConnInfo;

static void timer_cb(const boost::system::error_code& error, GlobalInfo* g);

/* Update the event timer after curl_multi library calls */
static int multi_timer_cb(CURLM* multi, long timeout_ms, GlobalInfo* g)
{
    fprintf(MSG_OUT, "\nmulti_timer_cb: timeout_ms %ld", timeout_ms);

    /* cancel running timer */
    timer.cancel();

    if (timeout_ms > 0)
    {
        /* update timer */
        timer.expires_from_now(boost::posix_time::millisec(timeout_ms));
        timer.async_wait(boost::bind(&timer_cb, _1, g));
    }
    else
    {
        /* call timeout function immediately */
        boost::system::error_code error; /*success*/
        timer_cb(error, g);
    }

    return 0;
}

/* Die if we get a bad CURLMcode somewhere */
static void mcode_or_die(const char* where, CURLMcode code)
{
    if (CURLM_OK != code)
    {
        const char* s;
        switch (code)
        {
        case CURLM_CALL_MULTI_PERFORM: s = "CURLM_CALL_MULTI_PERFORM"; break;
        case CURLM_BAD_HANDLE:         s = "CURLM_BAD_HANDLE";         break;
        case CURLM_BAD_EASY_HANDLE:    s = "CURLM_BAD_EASY_HANDLE";    break;
        case CURLM_OUT_OF_MEMORY:      s = "CURLM_OUT_OF_MEMORY";      break;
        case CURLM_INTERNAL_ERROR:     s = "CURLM_INTERNAL_ERROR";     break;
        case CURLM_UNKNOWN_OPTION:     s = "CURLM_UNKNOWN_OPTION";     break;
        case CURLM_LAST:               s = "CURLM_LAST";               break;
        default: s = "CURLM_unknown";
            break;
        case     CURLM_BAD_SOCKET:         s = "CURLM_BAD_SOCKET";
            fprintf(MSG_OUT, "\nERROR: %s returns %s", where, s);
            /* ignore this error */
            return;
        }
        fprintf(MSG_OUT, "\nERROR: %s returns %s", where, s);
        exit(code);
    }
}

/* Check for completed transfers, and remove their easy handles */
static void check_multi_info(GlobalInfo* g)
{
    char* eff_url;
    CURLMsg* msg;
    int msgs_left;
    ConnInfo* conn;
    CURL* easy;
    CURLcode res;

    fprintf(MSG_OUT, "\nREMAINING: %d", g->still_running);

    while ((msg = curl_multi_info_read(g->multi, &msgs_left)))
    {
        if (msg->msg == CURLMSG_DONE)
        {
            easy = msg->easy_handle;
            res = msg->data.result;
            curl_easy_getinfo(easy, CURLINFO_PRIVATE, &conn);
            curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &eff_url);
            fprintf(MSG_OUT, "\nDONE: %s => (%d) %s", eff_url, res, conn->error);
            curl_multi_remove_handle(g->multi, easy);
            free(conn->url);
            curl_easy_cleanup(easy);
            free(conn);
        }
    }
}

static void setsock(socket_ptr& tcp_socket, curl_socket_t s, CURL* e, int act, GlobalInfo* g);

/* Called by asio when there is an action on a socket */
static void event_cb(GlobalInfo* g, socket_ptr& tcp_socket, curl_socket_t s, CURL* e, int action, const boost::system::error_code& err)
{
    CURLMcode rc;

    assert(tcp_socket->sock.native_handle() == s);

    if (err)
    {
        fprintf(MSG_OUT, "\nevent_cb: socket=%d action=%d \nERROR=%s", s, action, err.message().c_str());
        rc = curl_multi_socket_action(g->multi, tcp_socket->sock.native_handle(), CURL_CSELECT_ERR, &g->still_running);
    }
    else
    {
        fprintf(MSG_OUT, "\nevent_cb: socket=%d action=%d", s, action);
        rc = curl_multi_socket_action(g->multi, tcp_socket->sock.native_handle(), action, &g->still_running);
    }

    mcode_or_die("event_cb: curl_multi_socket_action", rc);
    check_multi_info(g);

    if (g->still_running <= 0)
    {
        fprintf(MSG_OUT, "\nlast transfer done, kill timeout");
        timer.cancel();
    }
    else
    {
        int action_continue = (tcp_socket->mask) & action;
        if (action_continue)
        {
            fprintf(MSG_OUT, "\ncontinue read or write: %d", action_continue);
            setsock(tcp_socket, s, e, action_continue, g); // continue read or write
        }
    }
}

/* Called by asio when our timeout expires */
static void timer_cb(const boost::system::error_code& error, GlobalInfo* g)
{
    if (!error)
    {
        fprintf(MSG_OUT, "\ntimer_cb: ");

        CURLMcode rc;
        rc = curl_multi_socket_action(g->multi, CURL_SOCKET_TIMEOUT, 0, &g->still_running);

        mcode_or_die("timer_cb: curl_multi_socket_action", rc);
        check_multi_info(g);
    }
}

static void setsock(socket_ptr& tcp_socket, curl_socket_t s, CURL* e, int act, GlobalInfo* g)
{
    fprintf(MSG_OUT, "\nsetsock: socket=%d, act=%d ", s, act);
    assert(tcp_socket->sock.native_handle() == s);

    if (act == CURL_POLL_IN)
    {
        fprintf(MSG_OUT, "\nwatching for socket to become readable");

        tcp_socket->sock.async_read_some(boost::asio::null_buffers(),
            boost::bind(&event_cb, g,
                tcp_socket, s, e,
                act, _1));
    }
    else if (act == CURL_POLL_OUT)
    {
        fprintf(MSG_OUT, "\nwatching for socket to become writable");

        tcp_socket->sock.async_write_some(boost::asio::null_buffers(),
            boost::bind(&event_cb, g,
                tcp_socket, s, e,
                act, _1));
    }
    else if (act == CURL_POLL_INOUT)
    {
        fprintf(MSG_OUT, "\nwatching for socket to become readable & writable");

        tcp_socket->sock.async_read_some(boost::asio::null_buffers(),
            boost::bind(&event_cb, g,
                tcp_socket, s, e,
                CURL_POLL_IN, _1));

        tcp_socket->sock.async_write_some(boost::asio::null_buffers(),
            boost::bind(&event_cb, g,
                tcp_socket, s, e,
                CURL_POLL_OUT, _1));
    }
}

/* CURLMOPT_SOCKETFUNCTION */
static int multi_sock_cb(CURL* e, curl_socket_t s, int what, void* cbp, void* sockp)
{
    fprintf(MSG_OUT, "\nmulti_sock_cb: socket=%d, what=%d, sockp=%p", s, what, sockp);

    GlobalInfo* g = (GlobalInfo*)cbp;
    int* actionp = (int*)sockp;
    const char* whatstr[] = { "none", "IN", "OUT", "INOUT", "REMOVE" };

    fprintf(MSG_OUT,
        "\nsocket callback: s=%d e=%p what=%s ", s, e, whatstr[what]);

    std::map<curl_socket_t, socket_ptr>::iterator it = socket_map.find(s);

    if (it == socket_map.end())
    {
        if (actionp)
        {
            fprintf(MSG_OUT, "\nsocket closed already before remove CURL_POLL_REMOVE event. bug???", s);
            //*actionp = what;
            return 0;
        }
        else
        {
            fprintf(MSG_OUT, "\nwe don't know how to create asio::ip::tcp::socket without this fd's protocol family, please recompiled libcurl without c-ares\n");
            fprintf(MSG_OUT, "\nsocket %d is a c-ares socket, ignoring", s);
            return 0; // don't poll this fd, will cause c-ares read dns response until timeout
        }

    }

    socket_ptr& tcp_socket = it->second;

    if (!actionp)
    {
        actionp = &(tcp_socket->mask);

        curl_multi_assign(g->multi, s, actionp);
    }

    if (what == CURL_POLL_REMOVE)
    {
        fprintf(MSG_OUT, "\n");
        fprintf(MSG_OUT, "\nremsock: socket=%d", s);
    }
    else
    {
        fprintf(MSG_OUT,
            "\nChanging action from %s to %s",
            whatstr[*actionp], whatstr[what]);
        setsock(tcp_socket, s, e, what & (~*actionp), g); // only add new instrest
    }

done:
    *actionp = what;
    return 0;
}


/* CURLOPT_WRITEFUNCTION */
static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* data)
{

    size_t written = size * nmemb;
    char* pBuffer = (char*)malloc(written + 1);

    strncpy(pBuffer, (const char*)ptr, written);
    pBuffer[written] = '\0';

    fprintf(MSG_OUT, "\nfetch %u bytes\n", written);
    fprintf(MSG_OUT, "%s", pBuffer);

    free(pBuffer);

    return written;
}


/* CURLOPT_PROGRESSFUNCTION */
static int prog_cb(void* p, double dltotal, double dlnow, double ult,
    double uln)
{
    ConnInfo* conn = (ConnInfo*)p;
    (void)ult;
    (void)uln;

    fprintf(MSG_OUT, "\nProgress: %s (%g/%g)", conn->url, dlnow, dltotal);
    fprintf(MSG_OUT, "\nProgress: %s (%g)", conn->url, ult);

    return 0;
}

/* CURLOPT_OPENSOCKETFUNCTION */
static curl_socket_t opensocket(void* clientp,
    curlsocktype purpose,
    struct curl_sockaddr* address)
{
    fprintf(MSG_OUT, "\nopensocket :");

    curl_socket_t sockfd = CURL_SOCKET_BAD;

    /* restrict to ipv4 */
    if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
    {
        /* create a tcp socket object */
        SocketInfo* tcp_socket = new SocketInfo(io_service);

        /* open it and get the native handle*/
        boost::system::error_code ec;
        tcp_socket->sock.open(boost::asio::ip::tcp::v4(), ec);

        if (ec)
        {
            //An error occurred
            std::cout << std::endl << "Couldn't open socket [" << ec << "][" << ec.message() << "]";
            fprintf(MSG_OUT, "\nERROR: Returning CURL_SOCKET_BAD to signal error");
        }
        else
        {
            sockfd = tcp_socket->sock.native_handle();
            fprintf(MSG_OUT, "\nOpened socket %d", sockfd);

            /* save it for monitoring */
            size_t size = socket_map.size();
            socket_map.insert(std::pair<curl_socket_t, socket_ptr>(sockfd, socket_ptr(tcp_socket)));
            assert(size + 1 == socket_map.size());
        }
    }

    return sockfd;
}

/* CURLOPT_CLOSESOCKETFUNCTION */
static int closesocket_cb(void* clientp, curl_socket_t item)
{
    fprintf(MSG_OUT, "\nclosesocket : %d", item);

    std::map<curl_socket_t, socket_ptr>::iterator it = socket_map.find(item);

    if (it != socket_map.end())
    {
        assert(it->second->sock.native_handle() == item);
        it->second->mask = 0;
        // close or cancel will cancel any asynchronous send, receive or connect operations 
        // Caution: on Windows platform, if connect host timeout, the event_cb will pending forever. Must be canceled manually
        it->second->sock.cancel();
        socket_map.erase(it);
    }
    else
        assert(false);

    return 0;
}

/* Create a new easy handle, and add it to the global curl_multi */
static void new_conn(const char* url, GlobalInfo* g)
{
    ConnInfo* conn;
    CURLMcode rc;

    conn = (ConnInfo*)calloc(1, sizeof(ConnInfo));
    memset(conn, 0, sizeof(ConnInfo));
    conn->error[0] = '\0';

    conn->easy = curl_easy_init();

    if (!conn->easy)
    {
        fprintf(MSG_OUT, "\ncurl_easy_init() failed, exiting!");
        exit(2);
    }
    conn->global = g;
    conn->url = strdup(url);
    curl_easy_setopt(conn->easy, CURLOPT_URL, conn->url);
    curl_easy_setopt(conn->easy, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(conn->easy, CURLOPT_WRITEDATA, &conn);
    curl_easy_setopt(conn->easy, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(conn->easy, CURLOPT_ERRORBUFFER, conn->error);
    curl_easy_setopt(conn->easy, CURLOPT_PRIVATE, conn);
    curl_easy_setopt(conn->easy, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(conn->easy, CURLOPT_PROGRESSFUNCTION, prog_cb);
    curl_easy_setopt(conn->easy, CURLOPT_PROGRESSDATA, conn);
    curl_easy_setopt(conn->easy, CURLOPT_LOW_SPEED_TIME, 3L);
    curl_easy_setopt(conn->easy, CURLOPT_LOW_SPEED_LIMIT, 10L);
    curl_easy_setopt(conn->easy, CURLOPT_CONNECTTIMEOUT, 16L); // timeout for the connect phase 

    /* call this function to get a socket */
    curl_easy_setopt(conn->easy, CURLOPT_OPENSOCKETFUNCTION, opensocket);

    /* call this function to close a socket */
    curl_easy_setopt(conn->easy, CURLOPT_CLOSESOCKETFUNCTION, closesocket_cb);

    fprintf(MSG_OUT,
        "\nAdding easy %p to multi %p (%s)", conn->easy, g->multi, url);
    rc = curl_multi_add_handle(g->multi, conn->easy);
    mcode_or_die("new_conn: curl_multi_add_handle", rc);

    /* note that the add_handle() will set a time-out to trigger very soon so
       that the necessary socket_action() call will be called by this app */
}

int run_asio_curl()
{
    GlobalInfo g;
    CURLMcode rc;
    /*(void)argc;
    (void)argv;*/

    memset(&g, 0, sizeof(GlobalInfo));
    g.multi = curl_multi_init();

    curl_multi_setopt(g.multi, CURLMOPT_SOCKETFUNCTION, multi_sock_cb);
    curl_multi_setopt(g.multi, CURLMOPT_SOCKETDATA, &g);
    curl_multi_setopt(g.multi, CURLMOPT_TIMERFUNCTION, multi_timer_cb);
    curl_multi_setopt(g.multi, CURLMOPT_TIMERDATA, &g);

    for (int idx = 1; idx < 2; ++idx)
    {
        new_conn("www.baidu.com", &g);  /* add a URL */
    }

    /* enter io_service run loop */
    io_service.run();

    curl_multi_cleanup(g.multi);

    fprintf(MSG_OUT, "\ndone.\n");
    return 0;
}