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
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include <boost/lexical_cast.hpp>
#include "control_connection.hpp"
#include "ftp_exception.hpp"

namespace ftp
{

using std::string;
using std::istream;

control_connection::control_connection(boost::asio::io_context & io_context)
    : socket_(io_context),
      resolver_(io_context)
{
}

void control_connection::open(const std::string & hostname, const std::string & port)
{
    boost::system::error_code ec;

    boost::asio::connect(socket_, resolver_.resolve(hostname, port, ec), ec);

    if (ec)
    {
        throw ftp_exception("cannot open control connection: %1%", ec.message());
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
        throw ftp_exception("cannot close control connection: %1%", ec.message());
    }
}

string control_connection::recv()
{
    string reply;
    string line;

    line = read_line();

    /**
     * A reply is defined to contain the 3-digit code, followed by Space
     * <SP>, followed by one line of text (where some maximum line length
     * has been specified), and terminated by the Telnet end-of-line code.
     *
     * Make sure the line contains at least 3-digit code and followed Space <SP>.
     *
     * RFC 959: https://tools.ietf.org/html/rfc959
     */
    if (line.size() < 4)
    {
        throw ftp_exception("invalid server reply: %1%", line);
    }

    uint16_t code;
    if (!try_parse_code(line, code))
    {
        throw ftp_exception("invalid server reply: %1%", line);
    }

    reply += line;

    /**
     * Thus the format for multi-line replies is that the first line
     * will begin with the exact required reply code, followed
     * immediately by a Hyphen, "-" (also known as Minus), followed by
     * text.
     *
     * RFC 959: https://tools.ietf.org/html/rfc959
     */
    if (line[3] == '-')
    {
        uint16_t reply_code = code;

        for (;;)
        {
            line = read_line();
            reply += '\n' + line;

            /**
             * The last line will begin with the same code, followed
             * immediately by Space <SP>, optionally some text, and the Telnet
             * end-of-line code.
             *
             * RFC 959: https://tools.ietf.org/html/rfc959
             */
            if (line.size() > 3 && line[3] == ' ' && try_parse_code(line, code))
            {
                if (reply_code == code)
                {
                    break;
                }
            }
        }
    }

    return reply;
}

void control_connection::send(const string & command)
{
    boost::system::error_code ec;

    boost::asio::write(socket_, boost::asio::buffer(command + "\r\n"), ec);

    if (ec)
    {
        throw ftp_exception("cannot send command: %1%", ec.message());
    }
}

string control_connection::read_line()
{
    string line;

    for (;;)
    {
        boost::system::error_code ec;
        char ch;

        size_t len = boost::asio::read(socket_, boost::asio::buffer(&ch, 1), ec);

        if (ec)
        {
            throw ftp_exception("cannot receive reply: %1%", ec.message());
        }

        if (len != 1)
        {
            break;
        }

        if (ch == '\r')
        {
            continue;
        }
        else if (ch == '\n')
        {
            break;
        }
        else
        {
            line += ch;
        }
    }

    return line;
}

bool control_connection::try_parse_code(const std::string & line, uint16_t & code)
{
    if (line.size() < 3)
    {
        return false;
    }

    try
    {
        code = boost::lexical_cast<uint16_t>(line.substr(0, 3));

        return true;
    }
    catch (const boost::bad_lexical_cast & ex)
    {
        return false;
    }
}

} // namespace ftp
