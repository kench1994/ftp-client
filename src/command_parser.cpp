/**
 * command_parser.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "command_parser.hpp"
#include "resources.hpp"
#include <sstream>

using std::istringstream;

namespace ftp
{

user_command command_parser::parse(const string & user_input)
{
    istringstream iss(user_input);
    string command;
    iss >> command;

    vector<string> parameters;
    for (string parameter; iss >> parameter;)
    {
        parameters.push_back(parameter);
    }

    return user_command(std::move(command), std::move(parameters));
}

} // namespace ftp
