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

namespace ftp
{

user_command command_parser::parse(const std::string & user_input)
{
    std::istringstream iss(user_input);
    std::string command;
    iss >> command;

    std::vector<std::string> parameters;
    for (std::string parameter; iss >> parameter;)
    {
        parameters.push_back(parameter);
    }

    return user_command(std::move(command), std::move(parameters));
}

} // namespace ftp
