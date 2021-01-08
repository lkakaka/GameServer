//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"

namespace http {
namespace server {

connection_manager::connection_manager() : max_conn_id(0)
{
}

void connection_manager::start(connection_ptr c)
{
  connections_.insert(c);
  int conn_id = alloc_connection_id();
  mp_connections.emplace(conn_id, c);
  c->set_conn_id(conn_id);
  c->start();
}

void connection_manager::stop(connection_ptr c)
{
  connections_.erase(c);
  c->stop();
}

void connection_manager::stop_all()
{
  for (auto c: connections_)
    c->stop();
  connections_.clear();
}

connection_ptr connection_manager::get_connection(int conn_id) {
    auto iter = mp_connections.find(conn_id);
    if (iter == mp_connections.end()) {
        return NULL;
    }
    return iter->second;
}

} // namespace server
} // namespace http
