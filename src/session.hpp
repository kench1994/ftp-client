/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#ifndef FTP_CLIENT_SESSION_HPP
#define FTP_CLIENT_SESSION_HPP

#include <boost/asio/ip/tcp.hpp>

using std::string;

using namespace boost;

namespace ftp
{

class session
{
public:

    session();

    void open_control_connection(const string & hostname, const string & port);

private:
    asio::io_context io_context_;
    asio::ip::tcp::socket control_connection_;
    asio::ip::tcp::resolver resolver_;
};

} // namespace ftp
#endif //FTP_CLIENT_SESSION_HPP
