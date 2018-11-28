/**
 * control_connection.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_CONTROL_CONNECTION_HPP
#define FTP_CLIENT_CONTROL_CONNECTION_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

using namespace boost;

namespace ftp
{

class control_connection
{
public:
    control_connection(const std::string & hostname, const std::string & port);

    ~control_connection();

    std::string read();

    void write(const std::string & command);

private:
    std::string read_line();

    std::string get_reply_code(const std::string & reply);

    bool is_multiline_reply(const std::string & reply) const;

    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    asio::ip::tcp::resolver resolver_;
    asio::streambuf read_buf_;
};

} // namespace ftp
#endif //FTP_CLIENT_CONTROL_CONNECTION_HPP
