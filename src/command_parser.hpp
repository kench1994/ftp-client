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

using std::vector;
using std::string;

namespace ftp
{

class command_parser
{
public:
    static vector<string> parse(const string & command);
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_PARSER_HPP
