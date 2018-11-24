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

using std::string;

using namespace boost;

namespace ftp
{

class control_connection
{
public:
    control_connection(const string & hostname, const string & port);

    ~control_connection();

    string read();

    void write(const string & command);

private:
    string read_line();

    string get_reply_code(const string & reply);

    bool is_multiline_reply(const string & reply) const;

    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    asio::ip::tcp::resolver resolver_;
    asio::streambuf read_buf_;
};

} // namespace ftp
#endif //FTP_CLIENT_CONTROL_CONNECTION_HPP
