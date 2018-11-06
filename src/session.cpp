/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include "session.hpp"

using std::istream;

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

void session::close_control_connection()
{
    control_connection_.close();
}

string session::read_control_connection()
{
    string response;
    string line;

    do
    {
        asio::read_until(control_connection_, read_buf_, '\n');
        istream is(&read_buf_);
        getline(is, line);
        response += (line + "\n");
    }
    while (line[3] == '-');

    return response;
}

void session::write_control_connection(const string & request)
{
    asio::write(control_connection_, asio::buffer(request + "\r\n"));
}

bool session::control_connection_is_open() const
{
    return control_connection_.is_open();
}

} // namespace ftp
