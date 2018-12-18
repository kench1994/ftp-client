/**
 * client.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_CLIENT_HPP
#define FTP_CLIENT_CLIENT_HPP

#include <string>
#include <vector>
#include "control_connection.hpp"
#include <optional>

namespace ftp
{

class client
{
public:
    void open(const std::string & hostname, const std::string & port = "21");

    bool is_open() const;

    void user(const std::string & username);

    void pass(const std::string & password);

    void list(const std::optional<std::string> & remote_directory = std::nullopt);

    void close();

    void reset();

private:
    boost::asio::ip::tcp::endpoint parse_pasv_reply(const std::string & reply);

    std::string pasv();

    std::unique_ptr<control_connection> control_connection_;
};

} // namespace ftp
#endif //FTP_CLIENT_CLIENT_HPP
