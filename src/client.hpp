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

namespace ftp
{

class client
{
public:
    void execute_command(const std::string & command,
                         const std::vector<std::string> & arguments);

    bool is_open() const;

    void reset();

private:
    bool is_needed_connection(const std::string & command) const;

    boost::asio::ip::tcp::endpoint parse_pasv_reply(const std::string & reply);

    void open(const std::vector<std::string> & arguments);

    void user(const std::vector<std::string> & arguments = {});

    void ls(const std::vector<std::string> & arguments);

    std::string pasv();

    void close();

    void help();

    void exit();

    std::unique_ptr<control_connection> control_connection_;
};

} // namespace ftp
#endif //FTP_CLIENT_CLIENT_HPP
