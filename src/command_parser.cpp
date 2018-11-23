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

static bool is_valid_command(const string & command)
{
    return command == command::open ||
           command == command::close ||
           command == command::help ||
           command == command::exit;
}

user_command command_parser::parse(const string & user_input)
{
    istringstream iss(user_input);
    string command;
    iss >> command;

    if (!is_valid_command(command))
    {
        throw std::runtime_error(error::invalid_command);
    }

    vector<string> parameters;
    for (string parameter; iss >> parameter;)
    {
        parameters.push_back(parameter);
    }

    return user_command(std::move(command), std::move(parameters));
}

} // namespace ftp
