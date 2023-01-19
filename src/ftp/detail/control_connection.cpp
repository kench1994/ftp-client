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

#include "control_connection.hpp"
#include "connection_exception.hpp"
#include <boost/asio/connect.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>
#include <iostream>
namespace ftp::detail
{

using std::uint16_t;
using std::string;
using std::to_string;

static bool try_parse_status_code(const string & line, uint16_t & status_code)
{
    if (line.size() < 3)
    {
        return false;
    }

    return boost::conversion::try_lexical_convert(line.substr(0, 3), status_code);
}

control_connection::control_connection()
    : io_context_(),
      socket_(io_context_)
{
}

void control_connection::open(const string & hostname, uint16_t port)
{
    boost::system::error_code ec;
    //TODO: ipv6 address support
    //"fe80::1205:14e1:f17a:8b8a%ens33"
    //boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string("fe80::1205:14e1:f17a:8b8a%ens33"), 20182);

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(hostname), 20182);
    socket_.connect(endpoint, ec);
    if (ec)
    {
        boost::system::error_code ignored;
        std::cerr << ec.message() << std::endl;
        /* If the connect fails, and the socket was automatically opened,
         * the socket is not returned to the closed state.
         *
         * https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/reference/basic_stream_socket/connect/overload2.html
         */
        socket_.close(ignored);

        throw connection_exception(ec, "Cannot open connection");
    }
}

bool control_connection::is_open() const
{
    return socket_.is_open();
}

void control_connection::close()
{
    boost::system::error_code ec;

    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

    if (ec == boost::asio::error::not_connected)
    {
        /* Ignore 'not_connected' error. We could get ENOTCONN if an server side
         * has already closed the control connection. This suits us, just close
         * the socket.
         */
    }
    else if (ec)
    {
        throw connection_exception(ec, "Cannot close connection");
    }

    socket_.close(ec);

    if (ec)
    {
        throw connection_exception(ec, "Cannot close connection");
    }
}

string control_connection::ip() const
{
    boost::system::error_code ec;

    boost::asio::ip::tcp::endpoint remote_endpoint = socket_.remote_endpoint(ec);

    if (ec)
    {
        throw connection_exception(ec, "Cannot get ip address");
    }

    string ip = remote_endpoint.address().to_string(ec);

    if (ec)
    {
        throw connection_exception(ec, "Cannot get ip address");
    }

    return ip;
}

reply_t control_connection::recv()
{
    uint16_t status_code;
    string status_line;

    status_line = read_line();

    if (!try_parse_status_code(status_line, status_code))
    {
        throw connection_exception("Invalid server reply: %1%", status_line);
    }

    /* Thus the format for multi-line replies is that the first line
     * will begin with the exact required reply code, followed
     * immediately by a Hyphen, "-" (also known as Minus), followed by
     * text.
     *
     * RFC 959: https://tools.ietf.org/html/rfc959
     */
    if (status_line.size() > 3 && status_line[3] == '-')
    {
        for (;;)
        {
            string line = read_line();

            status_line += line;

            if (is_last_line(line, status_code))
            {
                break;
            }
        }
    }

    /* Handle 421 (service not available, closing control connection) code as
     * a generic error. This may be a reply to any command if the service knows
     * it must shut down.
     */
    if (status_code == 421)
    {
        boost::system::error_code ec;

        socket_.close(ec);

        if (ec)
        {
            throw connection_exception(ec, "Cannot close connection");
        }
    }

    return reply_t(status_code, status_line);
}

/* The last line will begin with the same code, followed
 * immediately by Space <SP>, optionally some text, and the Telnet
 * end-of-line code.
 *
 * RFC 959: https://tools.ietf.org/html/rfc959
 */
bool control_connection::is_last_line(const string & line, uint16_t status_code)
{
    if (line.size() < 4)
    {
        return false;
    }

    if (line[3] != ' ')
    {
        return false;
    }

    uint16_t code;
    if (!try_parse_status_code(line, code))
    {
        return false;
    }

    return code == status_code;
}

void control_connection::send(const string & command)
{
    boost::system::error_code ec;

    boost::asio::write(socket_, boost::asio::buffer(command + "\r\n"), ec);

    if (ec)
    {
        throw connection_exception(ec, "Cannot send command");
    }
}

string control_connection::read_line()
{
    boost::system::error_code ec;

    size_t len = boost::asio::read_until(socket_, boost::asio::dynamic_buffer(buffer_), '\n', ec);

    if (ec == boost::asio::error::eof)
    {
        /* Ignore eof. */
    }
    else if (ec)
    {
        throw connection_exception(ec, "Cannot receive reply");
    }

    string line = buffer_.substr(0, len);
    buffer_.erase(0, len);

    return line;
}

} // namespace ftp::detail
