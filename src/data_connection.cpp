/**
 * data_connection.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "data_connection.hpp"
#include <boost/asio/read.hpp>

namespace ftp
{

using std::string;
using std::istream;

data_connection::data_connection(const boost::asio::ip::tcp::endpoint & endpoint)
    : endpoint_(endpoint),
      socket_(io_context_, endpoint_.protocol())
{
}

void data_connection::connect()
{
    socket_.connect(endpoint_);
}

string data_connection::read()
{
    try
    {
        boost::asio::read(socket_, read_buf_);
    }
    catch (const boost::system::system_error & ex)
    {
        if (ex.code() != boost::asio::error::eof)
        {
            throw;
        }
    }

    istream is(&read_buf_);
    string multiline_reply;
    string reply_line;

    while (getline(is, reply_line))
    {
        multiline_reply += "\n" + reply_line;
    }

    return multiline_reply;
}

} // namespace ftp
