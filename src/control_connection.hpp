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

    bool is_multiline_reply(const std::string & reply_line) const;

    bool is_end_of_multiline_reply(const std::string & first_reply_line,
                                   const std::string & current_reply_line) const;

    bool is_negative_completion_code(const std::string & reply_line) const;

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::streambuf read_buf_;
};

} // namespace ftp
#endif //FTP_CLIENT_CONTROL_CONNECTION_HPP
