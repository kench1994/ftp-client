/**
 * data_connection.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "data_connection.hpp"
#include <boost/asio/read.hpp>
#include <fstream>
#include <boost/asio/buffers_iterator.hpp>

namespace ftp
{

using std::string;
using std::istream;
using std::ofstream;
using std::array;

data_connection::data_connection(boost::asio::io_context & io_context,
                                 const boost::asio::ip::tcp::endpoint & endpoint)
    : endpoint_(endpoint),
      socket_(io_context, endpoint_.protocol())
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
        boost::asio::read(socket_, stream_buffer_);
    }
    catch (const boost::system::system_error & ex)
    {
        if (ex.code() != boost::asio::error::eof)
        {
            throw;
        }
    }

    string reply = string(boost::asio::buffers_begin(stream_buffer_.data()),
                          boost::asio::buffers_end(stream_buffer_.data()));

    // Remove last '\r\n' characters.
    reply.pop_back();
    reply.pop_back();

    return reply;
}

void data_connection::read_file(ofstream & file)
{
    for (;;)
    {
        boost::system::error_code error;

        size_t len = socket_.read_some(boost::asio::buffer(buffer_), error);

        if (error == boost::asio::error::eof)
        {
            break;
        }
        else if (error)
        {
            throw boost::system::system_error(error);
        }

        file.write(buffer_.data(), len);
    }
}

} // namespace ftp
