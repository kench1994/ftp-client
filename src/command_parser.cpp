/**
 * command_parser.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "command_parser.hpp"
#include <sstream>

using std::istringstream;

namespace ftp
{

vector<string> command_parser::parse(const string & command)
{
    vector<string> parsed_command;
    istringstream iss(command);

    for (string parameter; iss >> parameter;)
    {
        parsed_command.push_back(parameter);
    }

    return parsed_command;
}

} // namespace ftp
