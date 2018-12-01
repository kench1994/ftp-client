/**
 * command_handler.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_COMMAND_HANDLER_HPP
#define FTP_CLIENT_COMMAND_HANDLER_HPP

#include <string>
#include <vector>
#include "control_connection.hpp"

namespace ftp
{

class command_handler
{
public:
    void execute(const std::string & command,
                 const std::vector<std::string> & arguments);

private:
    bool is_needed_connection(const std::string & command) const;

    void login();

    void login(const std::string & username);

    void open(const std::string & hostname, const std::string & port);

    void user(const std::string & username);

    void pass(const std::string & password);

    void close();

    void help();

    void exit();

    std::unique_ptr<control_connection> control_connection_;
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_HANDLER_HPP
