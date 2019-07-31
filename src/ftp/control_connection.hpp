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

#ifndef FTP_CLIENT_CONTROL_CONNECTION_HPP
#define FTP_CLIENT_CONTROL_CONNECTION_HPP

#include <boost/asio/ip/tcp.hpp>

namespace ftp
{

struct reply_t
{
    std::uint16_t code;
    std::string status_line;
};

class control_connection
{
public:
    control_connection();

    control_connection(const control_connection &) = delete;

    control_connection & operator=(const control_connection &) = delete;

    void open(const std::string & hostname, uint16_t port);

    bool is_open() const;

    void close();

    std::string ip() const;

    void send(const std::string & command);

    reply_t recv();

private:
    std::string read_line();

    bool is_last_line(const std::string & line, uint16_t code);

    std::string buffer_;
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
};

} // namespace ftp
#endif //FTP_CLIENT_CONTROL_CONNECTION_HPP
