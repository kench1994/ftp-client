/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
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

} // namespace ftp
