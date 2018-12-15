/**
 * command_handler.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_COMMAND_HANDLER_HPP
#define FTP_CLIENT_COMMAND_HANDLER_HPP

#include <string>
#include <vector>

namespace ftp
{

class command_handler
{
public:
    void execute(const std::string & command,
                 const std::vector<std::string> & arguments);
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_HANDLER_HPP
