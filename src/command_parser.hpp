/**
 * command_parser.hpp
 *
 * Copyright (c) 2019, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_COMMAND_PARSER_HPP
#define FTP_CLIENT_COMMAND_PARSER_HPP

#include <vector>
#include <string>

namespace ftp
{

class command_parser
{
public:
    explicit command_parser(const std::string & command);

    const std::string & get_command() const;

    const std::vector<std::string> & get_args() const;

private:
    std::string command_;
    std::vector<std::string> args_;
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_PARSER_HPP
