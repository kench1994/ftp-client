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
#include <boost/asio/streambuf.hpp>

namespace ftp
{

class control_connection
{
public:
    control_connection(boost::asio::io_context & io_context,
                       const std::string & hostname, const std::string & port);

    std::string read();

    void write(const std::string & command);

private:
    std::string read_line();

    bool is_end_of_multiline_reply(const std::string & first_reply,
                                   const std::string & current_reply) const;

    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    std::string string_buffer_;
};

} // namespace ftp
#endif //FTP_CLIENT_CONTROL_CONNECTION_HPP
