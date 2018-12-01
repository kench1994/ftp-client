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
#include "user_command.hpp"

namespace ftp
{

class command_handler
{
public:
    void execute(const user_command & user_command);

private:
    bool is_needed_connection(const user_command & command) const;

    void open(const std::vector<std::string> & parameters);

    void close();

    void help();

    void exit();

    std::unique_ptr<control_connection> control_connection_;
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_HANDLER_HPP
