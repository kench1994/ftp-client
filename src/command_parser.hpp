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
    static void process(const std::string & source_line,
                        std::string & command,
                        std::vector<std::string> & args);
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_PARSER_HPP
