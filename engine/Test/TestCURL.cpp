//#define CURL_STATICLIB                 //����Ǿ�̬�ⷽʽ����Ҫ�������

#include "curl/curl.h"
#include <string>
#include <iostream>
#include <list>
#include "Test.h"

//#ifdef _DEBUG
//#pragma comment(lib, "libcurld.lib")
//#else
//#pragma comment(lib, "libcurl.lib")
//#endif

//#pragma comment (lib, "ws2_32.lib")
//#pragma comment (lib, "winmm.lib")
//#pragma comment (lib, "wldap32.lib")
//#pragma comment(lib, "Advapi32.lib")

//const char* url = "http://www.baidu.com";
const char* url = "http://localhost:8028/1?cmd=gm_list";
const int count = 1;

size_t write_data(void* buffer, size_t size, size_t count, void* stream) {
    (void)buffer;
    (void)stream;
    printf("%s", buffer);
    return size * count;
}

void curl_multi_demo() {
    CURLM* curlm = curl_multi_init();

    for (int i = 0; i < count; ++i) {
        CURL* easy_handle = curl_easy_init();
        curl_easy_setopt(easy_handle, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(easy_handle, CURLOPT_URL, url);
        curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);

        curl_multi_add_handle(curlm, easy_handle);
    }

    int running_handlers = 0;
    do {
        curl_multi_wait(curlm, NULL, 0, 2000, NULL);
        curl_multi_perform(curlm, &running_handlers);
    } while (running_handlers > 0);

    int msgs_left = 0;
    CURLMsg* msg = NULL;
    while ((msg = curl_multi_info_read(curlm, &msgs_left)) != NULL) {
        if (msg->msg == CURLMSG_DONE) {
            int http_status_code = 0;
            curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &http_status_code);
            char* effective_url = NULL;
            curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &effective_url);
            fprintf(stdout, "url:%s status:%d %s\n",
                effective_url,
                http_status_code,
                curl_easy_strerror(msg->data.result));

            curl_multi_remove_handle(curlm, msg->easy_handle);
            curl_easy_cleanup(msg->easy_handle);
        }
    }

    curl_multi_cleanup(curlm);
}

//int main() {
//    curl_multi_demo();
//    return 0;
//}

//
//std::wstring AsciiToUnicode(const std::string& str)
//{
//    // Ԥ��-�������п��ֽڵĳ���  
//    int unicodeLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
//    // ��ָ�򻺳�����ָ����������ڴ�  
//    wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
//    // ��ʼ�򻺳���ת���ֽ�  
//    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pUnicode, unicodeLen);
//    std::wstring ret_str = pUnicode;
//    free(pUnicode);
//    return ret_str;
//}
//
//std::string UnicodeToUtf8(const std::wstring& wstr)
//{
//    // Ԥ��-�������ж��ֽڵĳ���  
//    int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
//    // ��ָ�򻺳�����ָ����������ڴ�  
//    char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
//    // ��ʼ�򻺳���ת���ֽ�  
//    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);
//    std::string ret_str = pAssii;
//    free(pAssii);
//    return ret_str;
//}
//
////ANSIתUTF8
//std::string AsciiToUtf8(const std::string& str)
//{
//    return UnicodeToUtf8(AsciiToUnicode(str));
//}
//
////UTF8תANSI
//std::string Utf8toAscii(const std::string strUTF8)
//{
//    std::string  strAnsi = "";
//    //��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�����
//    UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.c_str(), -1, NULL, NULL);
//    WCHAR* wszBuffer = new WCHAR[nLen + 1];
//    nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.c_str(), -1, wszBuffer, nLen);
//    wszBuffer[nLen] = 0;
//    nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
//    CHAR* szBuffer = new CHAR[nLen + 1];
//    nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
//    szBuffer[nLen] = 0;
//    strAnsi = szBuffer;
//    //�����ڴ�
//    delete[]szBuffer;
//    delete[]wszBuffer;
//    return strAnsi;
//}

// reply of the requery  
size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream)
{
    if (stream == NULL || ptr == NULL || size == 0)
        return 0;

    size_t realsize = size * nmemb;
    std::string* buffer = (std::string*)stream;
    if (buffer != NULL)
    {
        buffer->append((const char*)ptr, realsize);
    }
    return realsize;
    /*
    std::string *str = (std::string*)stream;
    (*str).append((char*)ptr, size*nmemb);
    return size * nmemb;
    */
}

/*
���ܣ�get http����
������url�������ַ��������������������ݣ�ֱ��ƴ�յ�url���棻���磺http://127.0.0.1:8080/api/Accounts/Login?uername=admin&password=123
listRequestHeader������ͷ�����б�
bResponseIsWithHeaderData��bool���ͣ���ʾ��Ӧ�����Ƿ����Ӧ��ͷ���ݡ�true��������false������������������Ļ���Ӧ�������а���Content-Type��Server����Ϣ��
nConnectTimeout�����ӳ�ʱʱ�䣬��λΪ�룻
nTimeout����д���ݳ�ʱʱ�䣬��λΪ��
����ֵ��CURLcode
*/
CURLcode curl_get_req(const std::string& url, std::string& response, std::list<std::string> listRequestHeader, bool bResponseIsWithHeaderData = false, int nConnectTimeout = 10, int nTimeout = 10)
{
    // init curl  
    CURL* curl = curl_easy_init();
    // res code  
    CURLcode res;
    if (curl)
    {
        // set params  
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
        //curl_easy_setopt(m_curl, CURLOPT_PORT, 8089);    //port
        curl_easy_setopt(curl, CURLOPT_POST, 0); // get reqest 
        //����HTTP����ͷ
        struct curl_slist* headers = NULL;
        if (listRequestHeader.size() > 0)
        {
            std::list<std::string>::iterator iter, iterEnd;
            iter = listRequestHeader.begin();
            iterEnd = listRequestHeader.end();
            for (iter; iter != iterEnd; iter++)
            {
                headers = curl_slist_append(headers, iter->c_str());
            }
            //headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
            //headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
            if (headers != NULL)
            {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//����http����ͷ��Ϣ
            }
        }
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        if (bResponseIsWithHeaderData)
        {
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);//��Ӧ�����Ƿ������ͷ��Ϣ������Content-Type:application/json;charset=UTF-8
        }
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, nConnectTimeout); // set transport and time out time  
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, nTimeout);
        // start request  
        res = curl_easy_perform(curl);
        if (headers != NULL)
        {
            curl_slist_free_all(headers); //free the list again
        }
    }
    // release curl  
    curl_easy_cleanup(curl);
    return res;
}

CURLcode curl_get_req_ex(CURL* curl, const std::string& url, std::string& response, std::list<std::string> listRequestHeader, bool bResponseIsWithHeaderData = false, int nConnectTimeout = 10, int nTimeout = 10)
{
    // res code  
    CURLcode res;
    if (curl)
    {
        // set params 
        curl_easy_reset(curl);
        /* enable TCP keep-alive for this transfer */
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        /* keep-alive idle time to 120 seconds */
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
        /* interval time between keep-alive probes: 30 seconds */
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 30L);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
        //curl_easy_setopt(m_curl, CURLOPT_PORT, 8089);    //port
        curl_easy_setopt(curl, CURLOPT_POST, 0); // get reqest 
        //����HTTP����ͷ
        struct curl_slist* headers = NULL;
        if (listRequestHeader.size() > 0)
        {
            std::list<std::string>::iterator iter, iterEnd;
            iter = listRequestHeader.begin();
            iterEnd = listRequestHeader.end();
            for (iter; iter != iterEnd; iter++)
            {
                headers = curl_slist_append(headers, iter->c_str());
            }
            //headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
            //headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
            if (headers != NULL)
            {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//����http����ͷ��Ϣ
            }
        }
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        if (bResponseIsWithHeaderData)
        {
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);//��Ӧ�����Ƿ������ͷ��Ϣ������Content-Type:application/json;charset=UTF-8
        }
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, nConnectTimeout); // set transport and time out time  
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, nTimeout);
        // start request  
        res = curl_easy_perform(curl);
        if (headers != NULL)
        {
            curl_slist_free_all(headers); //free the list again
        }
    }
    return res;
}

/*
���ܣ�post http����
������url�������ַ��������磺http://127.0.0.1:8080/api/Accounts/Login
postParams�����󸽴��Ĳ���������uername=admin&password=123
listRequestHeader������ͷ�����б�
bResponseIsWithHeaderData��bool���ͣ���ʾ��Ӧ�����Ƿ����Ӧ��ͷ���ݡ�true��������false������������������Ļ���Ӧ�������а���Content-Type��Server����Ϣ��
nConnectTimeout�����ӳ�ʱʱ�䣬��λΪ�룻
nTimeout����д���ݳ�ʱʱ�䣬��λΪ��
����ֵ��CURLcode
*/
CURLcode curl_post_req(const std::string& url, const std::string& postParams, std::string& response, std::list<std::string> listRequestHeader, bool bResponseIsWithHeaderData = false, int nConnectTimeout = 10, int nTimeout = 10)
{
    // init curl  
    CURL* curl = curl_easy_init();
    // res code  
    CURLcode res;
    if (curl)
    {
        // set params  
        curl_easy_setopt(curl, CURLOPT_POST, 1); // post req  
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
        //curl_easy_setopt(m_curl, CURLOPT_PORT, 8089);    //port
        curl_easy_setopt(curl, CURLOPT_POST, 1); // post reqest 
        //����HTTP����ͷ
        struct curl_slist* headers = NULL;
        if (listRequestHeader.size() > 0)
        {
            std::list<std::string>::iterator iter, iterEnd;
            iter = listRequestHeader.begin();
            iterEnd = listRequestHeader.end();
            for (iter; iter != iterEnd; iter++)
            {
                headers = curl_slist_append(headers, iter->c_str());
            }
            //headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
            //headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
            if (headers != NULL)
            {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//����http����ͷ��Ϣ
            }
        }
        else
        {
            headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
            if (headers != NULL)
            {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//����http����ͷ��Ϣ
            }
        }
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str()); // params  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //���ص�ͷ������Location(һ��ֱ�������urlû�ҵ�)�����������Location��Ӧ������ 
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        if (bResponseIsWithHeaderData)
        {
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);//��Ӧ�����Ƿ������ͷ��Ϣ������Content-Type:application/json;charset=UTF-8
        }
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, nConnectTimeout);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, nTimeout);
        // start request  
        res = curl_easy_perform(curl);
        if (headers != NULL)
        {
            curl_slist_free_all(headers); //free the list again
        }
    }
    // release curl  
    curl_easy_cleanup(curl);
    return res;
}

CURLcode curl_post_req_ex(CURL* curl, const std::string& url, const std::string& postParams, std::string& response, std::list<std::string> listRequestHeader, bool bResponseIsWithHeaderData = false, int nConnectTimeout = 10, int nTimeout = 10)
{
    // res code  
    CURLcode res;
    if (curl)
    {
        // set params
        curl_easy_reset(curl);
        /* enable TCP keep-alive for this transfer */
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        /* keep-alive idle time to 120 seconds */
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
        /* interval time between keep-alive probes: 30 seconds */
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 30L);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
        //curl_easy_setopt(m_curl, CURLOPT_PORT, 8089);    //port
        curl_easy_setopt(curl, CURLOPT_POST, 1); // post reqest 
        //����HTTP����ͷ
        struct curl_slist* headers = NULL;
        if (listRequestHeader.size() > 0)
        {
            std::list<std::string>::iterator iter, iterEnd;
            iter = listRequestHeader.begin();
            iterEnd = listRequestHeader.end();
            for (iter; iter != iterEnd; iter++)
            {
                headers = curl_slist_append(headers, iter->c_str());
            }
            //headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
            //headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded;charset=UTF-8");
            if (headers != NULL)
            {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//����http����ͷ��Ϣ
            }
        }
        else
        {
            headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
            if (headers != NULL)
            {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//����http����ͷ��Ϣ
            }
        }
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str()); // params  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //���ص�ͷ������Location(һ��ֱ�������urlû�ҵ�)�����������Location��Ӧ������ 
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        if (bResponseIsWithHeaderData)
        {
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);//��Ӧ�����Ƿ������ͷ��Ϣ������Content-Type:application/json;charset=UTF-8
        }
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, nConnectTimeout);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, nTimeout);
        // start request  
        res = curl_easy_perform(curl);
        if (headers != NULL)
        {
            curl_slist_free_all(headers); //free the list again
        }
    }
    return res;
}

void test() {
    ////ʵ��1
    //curl_global_init(CURL_GLOBAL_ALL);

    ////post��ȡ����
    //std::string strResponse = "", strResponseAnsi = "";
    //strResponse.clear();
    //CURLcode res = curl_post_req("http://127.0.0.1:8080/api/Accounts/Login", "username=admin&password=123", strResponse);
    //if (res == CURLE_OK)
    //{
    //    std::string strToken = "";
    //    strResponseAnsi = Utf8toAscii(strResponse);
    //}

    ////get��ȡ����
    //strResponse.clear();
    //res = curl_get_req("http://127.0.0.1:8080/api/Accounts/Login?username=admin&password=123", strResponse);
    //if (res == CURLE_OK)
    //{
    //    int jj = 0;
    //}

    //curl_global_cleanup();


    //ʵ��2    
        //post json����
   /* CURL* curl = curl_easy_init();
    std::string strResponse = "", strResponseAnsi = "";
    char szRequestUrl[256] = { 0 };
    CURLcode res = CURLE_OK;
    sprintf_s(szRequestUrl, "%s/api/GPS/AddOne", "http://127.0.0.1:8080");
    std::string strPostParams = "";
    try
    {
        boost::property_tree::ptree ptroot;
        ptroot.put("deviceid", "12345678");
        ptroot.put<unsigned int>("deviceStatus", 0);
        ptroot.put<unsigned int>("alarmFlag", 0);
        ptroot.put("lng", fLongitude);
        ptroot.put("lat", fLatitude);
        ptroot.put("speed", 0);
        ptroot.put("direction", 0);
        ptroot.put<int>("altitude", 10);
        ptroot.put("gpsTime", "2018-10-10 12:00:01");
        std::stringstream sstream;
        boost::property_tree::write_json(sstream, ptroot);
        strPostParams = sstream.str();
        bSuccess = true;
    }
    catch (boost::property_tree::ptree_error pt)
    {
        pt.what();
    }
    if (bSuccess)
    {
        std::string strAuthorization = "admin---";
        std::string strRequestHeaders = strAuthorization;
        std::list<std::string> listRequestHeader;
        listRequestHeader.push_back(strRequestHeaders);
        listRequestHeader.push_back("Content-Type:application/json;charset=UTF-8");
        res = curl_post_req_ex(curl, szRequestUrl, strPostParams, strResponse, listRequestHeader);
        if (res == CURLE_OK)
        {
            bSuccess = true;
        }
    }

    curl_easy_cleanup(curl);*/
}