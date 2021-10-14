//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "http_client.h"
#include "StrUtil.h"
#include "HttpClientMgr.h"
#include "Logger.h"

HttpClient::HttpClient(boost::asio::io_context& io_context, const std::string& server, const std::string& path, HTTP_CLIENT_CB cb)
	: resolver_(io_context),
	socket_(io_context),
	m_callback(cb)
{
	// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	std::ostream request_stream(&request_);
	request_stream << "GET " << path << " HTTP/1.0\r\n";
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	int idx = server.find_first_of(":");
	std::string url = idx > 0 ? server.substr(0, idx) : server;
	std::string port = idx > 0 ? server.substr(idx + 1) : "";

	// Start an asynchronous resolve to translate the server and service names
	// into a list of endpoints.
	/*boost::asio::ip::tcp::resolver::query query("localhost", "8028");
	resolver_.async_resolve(query,
		boost::bind(&HttpClient::handle_resolve, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::results));*/

	resolver_.async_resolve(url, port,
		boost::bind(&HttpClient::handle_resolve, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::results));

	/*resolver_.async_resolve(server, "http",
		boost::bind(&HttpClient::handle_resolve, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::results));*/
}

void HttpClient::handle_resolve(const boost::system::error_code& err,
	const tcp::resolver::results_type& endpoints)
{
	if (!err)
	{
		// Attempt a connection to each endpoint in the list until we
		// successfully establish a connection.
		boost::asio::async_connect(socket_, endpoints,
			boost::bind(&HttpClient::handle_connect, this,
				boost::asio::placeholders::error));
	}
	else
	{
		LOG_ERROR("http client resolve error, %s", err.message().c_str());
		call_cb(err.message().c_str());
	}
}

void HttpClient::handle_connect(const boost::system::error_code& err)
{
	if (!err)
	{
		// The connection was successful. Send the request.
		boost::asio::async_write(socket_, request_,
			boost::bind(&HttpClient::handle_write_request, this,
				boost::asio::placeholders::error));
	}
	else
	{
		LOG_ERROR("http client connect error, %s", err.message().c_str());
		call_cb(err.message().c_str());
	}
}

void HttpClient::handle_write_request(const boost::system::error_code& err)
{
	if (!err)
	{
		// Read the response status line. The response_ streambuf will
		// automatically grow to accommodate the entire line. The growth may be
		// limited by passing a maximum size to the streambuf constructor.
		boost::asio::async_read_until(socket_, response_, "\r\n",
			boost::bind(&HttpClient::handle_read_status_line, this,
				boost::asio::placeholders::error));
	}
	else
	{	
		LOG_ERROR("http client write error, %s", err.message().c_str());
		call_cb(err.message().c_str());
	}
}

void HttpClient::handle_read_status_line(const boost::system::error_code& err)
{
	if (!err)
	{
		// Check that response is OK.
		std::istream response_stream(&response_);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			LOG_ERROR("http client Invalid response");
			call_cb(err.message().c_str());
			return;
		}
		reply_.status = (http::server::reply::status_type)status_code;
		if (status_code != 200)
		{
			/*std::cout << "Response returned with status code ";
			std::cout << status_code << "\n";*/
			call_cb(err.message().c_str());
			return;
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
			boost::bind(&HttpClient::handle_read_headers, this,
				boost::asio::placeholders::error));
	}
	else
	{
		LOG_ERROR("http client read status error, %s", err.message().c_str());
		call_cb(err.message().c_str());
	}
}

void HttpClient::handle_read_headers(const boost::system::error_code& err)
{
	if (!err)
	{
		// Process the response headers.
		std::istream response_stream(&response_);
		std::string header;
		while (std::getline(response_stream, header) && header != "\r") {
			//std::cout << header << "\n";
			int idx = header.find_first_of('=', 0);
			if (idx > 0) {
				http::server::header http_header;
				http_header.name = header.substr(0, idx);
				http_header.value = header.substr(idx + 1);
				reply_.headers.push_back(http_header);
			}
		}
		//std::cout << "\n";

		// Write whatever content we already have to output.
		if (response_.size() > 0) {
			//std::cout << &response_;
			std::istream response_stream(&response_);
			std::string content;
			while (std::getline(response_stream, content)) {
				reply_.content += content + "\n";
			}
		}

		// Start reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&HttpClient::handle_read_content, this,
				boost::asio::placeholders::error));
	}
	else
	{
		LOG_ERROR("http client read headers error, %s", err.message().c_str());
		call_cb(err.message().c_str());
	}
}

void HttpClient::handle_read_content(const boost::system::error_code& err)
{
	if (!err)
	{
		// Write all of the data that has been read so far.
		//std::cout << &response_;

		std::istream response_stream(&response_);
		std::string content;
		while (std::getline(response_stream, content)) {
			//LOG_INFO("%s", content.c_str());
			reply_.content += content + "\n";
		}

		// Continue reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&HttpClient::handle_read_content, this,
				boost::asio::placeholders::error));
	}
	else if (err != boost::asio::error::eof)
	{
		LOG_ERROR("http client read content error, %s", err.message().c_str());
		call_cb(err.message().c_str());
	}
	else {
		
		//LOG_INFO("%s", reply_.content.c_str());
		call_cb(NULL);
	}
}

void HttpClient::call_cb(const char* err) {
	if (m_callback != NULL) {
		if (err != NULL) {
			reply_.content = err;
		}
		m_callback(reply_.status, reply_.content);
	}
	HttpClientMgr::getSingleton()->removeClient(this);
}

void HttpClient::close() {
	try {
		socket_.close();
	} catch (std::exception& e) {
		LOG_ERROR("%s", e.what());
	}
}

//int main(int argc, char* argv[])
//{
//  try
//  {
//    if (argc != 3)
//    {
//      std::cout << "Usage: async_client <server> <path>\n";
//      std::cout << "Example:\n";
//      std::cout << "  async_client www.boost.org /LICENSE_1_0.txt\n";
//      return 1;
//    }
//
//    boost::asio::io_context io_context;
//    client c(io_context, argv[1], argv[2]);
//    io_context.run();
//  }
//  catch (std::exception& e)
//  {
//    std::cout << "Exception: " << e.what() << "\n";
//  }
//
//  return 0;
//}
