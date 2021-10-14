//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../reply.hpp"

using boost::asio::ip::tcp;

typedef std::function<void(int http_code, std::string& resp)> HTTP_CLIENT_CB;

class HttpClient
{
public:
    HttpClient(boost::asio::io_context& io_context, const std::string& server, const std::string& path, HTTP_CLIENT_CB cb);
    void close();

private:
    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
    http::server::reply reply_;
    
    HTTP_CLIENT_CB m_callback;

    void handle_resolve(const boost::system::error_code& err, const tcp::resolver::results_type& endpoints);
    void handle_connect(const boost::system::error_code& err);
    void handle_write_request(const boost::system::error_code& err);
    void handle_read_status_line(const boost::system::error_code& err);
    void handle_read_headers(const boost::system::error_code& err);
    void handle_read_content(const boost::system::error_code& err);

    void call_cb(const char* err);
};

