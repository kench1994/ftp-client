/**
 * command_parser.cpp
 *
 * Copyright (c) 2019, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "command_parser.hpp"
#include <sstream>
#include <iomanip>

namespace ftp
{

using std::vector;
using std::string;
using std::istringstream;

command_parser::command_parser(const std::string & command)
{
    istringstream iss(command);
    string arg;

    iss >> command_;

    while (iss >> quoted(arg))
    {
        args_.push_back(arg);
    }
}

const string & command_parser::get_command() const
{
    return command_;
}

const vector<string> & command_parser::get_args() const
{
    return args_;
}

} // namespace ftp
