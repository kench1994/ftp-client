/**
 * command_parser.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_COMMAND_PARSER_HPP
#define FTP_CLIENT_COMMAND_PARSER_HPP

#include <vector>
#include <string>
#include "user_command.hpp"

namespace ftp
{

class command_parser
{
public:
    static user_command parse(const std::string & user_input);
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_PARSER_HPP
