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

#include <boost/asio/connect.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>
#include "control_connection.hpp"
#include "ftp_exception.hpp"

namespace ftp
{

using std::uint16_t;
using std::string;
using std::to_string;

static bool try_parse_status_code(const string & line, uint16_t & code)
{
    if (line.size() < 3)
    {
        return false;
    }

    return boost::conversion::try_lexical_convert(line.substr(0, 3), code);
}

control_connection::control_connection()
    : io_context_(),
      socket_(io_context_)
{
}

void control_connection::open(const string & hostname, uint16_t port)
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::system::error_code ec;

    boost::asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(hostname, to_string(port), ec);

    if (ec)
    {
        throw ftp_exception("Cannot open connection: %1%", ec.message());
    }

    boost::asio::connect(socket_, endpoints, ec);

    if (ec)
    {
        boost::system::error_code ignored;

        /*
         * If the connect fails, and the socket was automatically opened,
         * the socket is not returned to the closed state.
         *
         * https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/reference/basic_stream_socket/connect/overload2.html
         */
        socket_.close(ignored);

        throw ftp_exception("Cannot open connection: %1%", ec.message());
    }
}

bool control_connection::is_open() const
{
    return socket_.is_open();
}

void control_connection::close()
{
    boost::system::error_code ec;

    socket_.close(ec);

    if (ec)
    {
        throw ftp_exception("Cannot close connection: %1%", ec.message());
    }
}

string control_connection::ip() const
{
    boost::system::error_code ec;

    boost::asio::ip::tcp::endpoint remote_endpoint = socket_.remote_endpoint(ec);

    if (ec)
    {
        throw ftp_exception("Cannot get ip address: %1%", ec.message());
    }

    string ip = remote_endpoint.address().to_string(ec);

    if (ec)
    {
        throw ftp_exception("Cannot get ip address: %1%", ec.message());
    }

    return ip;
}

reply_t control_connection::recv()
{
    uint16_t status_code;
    string status_line;

    status_line = read_line();

    /**
     * A reply is defined to contain the 3-digit code, followed by Space
     * <SP>, followed by one line of text.
     *
     * RFC 959: https://tools.ietf.org/html/rfc959
     */
    if (status_line.size() < 4)
    {
        throw ftp_exception("Invalid server reply: %1%", status_line);
    }

    if (!try_parse_status_code(status_line, status_code))
    {
        throw ftp_exception("Invalid server reply: %1%", status_line);
    }

    reply_t reply(status_code, status_line);

    /**
     * Thus the format for multi-line replies is that the first line
     * will begin with the exact required reply code, followed
     * immediately by a Hyphen, "-" (also known as Minus), followed by
     * text.
     *
     * RFC 959: https://tools.ietf.org/html/rfc959
     */
    if (status_line[3] == '-')
    {
        for (;;)
        {
            status_line = read_line();

            reply.status_line += status_line;

            if (is_last_line(status_line, reply.status_code))
            {
                break;
            }
        }
    }

    return reply;
}

/**
 * The last line will begin with the same code, followed
 * immediately by Space <SP>, optionally some text, and the Telnet
 * end-of-line code.
 *
 * RFC 959: https://tools.ietf.org/html/rfc959
 */
bool control_connection::is_last_line(const string & line, uint16_t reply_code)
{
    if (line.size() < 4)
    {
        return false;
    }

    if (line[3] != ' ')
    {
        return false;
    }

    uint16_t status_code;
    if (!try_parse_status_code(line, status_code))
    {
        return false;
    }

    return status_code == reply_code;
}

void control_connection::send(const string & command)
{
    boost::system::error_code ec;

    boost::asio::write(socket_, boost::asio::buffer(command + "\r\n"), ec);

    if (ec)
    {
        throw ftp_exception("Cannot send command: %1%", ec.message());
    }
}

string control_connection::read_line()
{
    boost::system::error_code ec;

    size_t len = boost::asio::read_until(socket_, boost::asio::dynamic_buffer(buffer_), '\n', ec);

    if (ec == boost::asio::error::eof)
    {
        // Ignore eof.
    }
    else if (ec)
    {
        throw ftp_exception("Cannot receive reply: %1%", ec.message());
    }

    string line = buffer_.substr(0, len);
    buffer_.erase(0, len);

    return line;
}

} // namespace ftp
