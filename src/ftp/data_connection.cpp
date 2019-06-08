/*
 * MIT License
 *
 * Copyright (c) 2019 Denis Kovalchuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "data_connection.hpp"
#include "ftp_exception.hpp"
#include <boost/asio/read.hpp>
#include <fstream>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/algorithm/string/trim.hpp>

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
    boost::system::error_code ec;

    socket_.connect(endpoint_, ec);

    if (ec)
    {
        throw ftp_exception("cannot create data connection: %1%", ec.message());
    }
}

string data_connection::recv()
{
    boost::system::error_code ec;

    boost::asio::read(socket_, stream_buffer_, ec);

    if (ec == boost::asio::error::eof)
    {
        // Ignore eof.
    }
    else if (ec)
    {
        throw ftp_exception("cannot receive reply: %1%", ec.message());
    }

    string reply = string(boost::asio::buffers_begin(stream_buffer_.data()),
                          boost::asio::buffers_end(stream_buffer_.data()));

    boost::algorithm::trim_if(reply, boost::algorithm::is_any_of("\r\n"));

    return reply;
}

void data_connection::recv_file(ofstream & file)
{
    for (;;)
    {
        boost::system::error_code ec;

        size_t len = socket_.read_some(boost::asio::buffer(buffer_), ec);

        if (ec == boost::asio::error::eof)
        {
            break;
        }
        else if (ec)
        {
            throw ftp_exception("cannot receive file: %1%", ec.message());
        }

        file.write(buffer_.data(), len);
    }
}

} // namespace ftp
