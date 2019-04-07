/**
 * data_connection.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_DATA_CONNECTION_HPP
#define FTP_CLIENT_DATA_CONNECTION_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

namespace ftp
{

class data_connection
{
public:
    data_connection(boost::asio::io_context & io_context,
                    const boost::asio::ip::tcp::endpoint & endpoint);

    void connect();

    std::string read();

    void read_file(std::ofstream & file);

private:
    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf stream_buffer_;
    std::array<char, 4096> buffer_;
};

} // namespace ftp
#endif //FTP_CLIENT_DATA_CONNECTION_HPP
