//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include "../Common/ServerExports.h"
#include "request.hpp"
#include "reply.hpp"
#include "IScriptRegister.h"

typedef http::server::reply_ptr(*CallHttpScripFunc)(void* server, int conn_id, const http::server::request& req);

namespace http {
namespace server {

/// The top-level class of the HTTP server.
class SERVER_EXPORT_API server : public IScriptRegister
{
public:
  server(const server&) = delete;
  server& operator=(const server&) = delete;

  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit server(const std::string& address, const std::string& port,
      const std::string& doc_root);

  /// Run the server's io_context loop.
  void run();

  inline connection_manager* get_connection_mgr() { return &connection_manager_; }

  inline void setCallHttpScripFunc(CallHttpScripFunc func) { m_scripFunc = func; }
  inline CallHttpScripFunc getCallHttpScripFunc() { return m_scripFunc; }

  ~server() {
      printf("server destory!!!!!!!\n");
  }

private:
    CallHttpScripFunc m_scripFunc;
  /// Perform an asynchronous accept operation.
  void do_accept();

  /// Wait for a request to stop the server.
  void do_await_stop();

  /// The io_context used to perform asynchronous operations.
  boost::asio::io_context io_context_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The connection manager which owns all live connections.
  connection_manager connection_manager_;

  /// The handler for all incoming requests.
  request_handler request_handler_;
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP
