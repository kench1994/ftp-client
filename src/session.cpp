/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#include <boost/asio/connect.hpp>
#include "session.hpp"

namespace ftp
{

session::session()
        : control_connection_(io_context_),
          resolver_(io_context_)
{
}

void session::open_control_connection(const string & hostname, const string & port)
{
    asio::connect(control_connection_, resolver_.resolve(hostname, port));
}

} // namespace ftp
