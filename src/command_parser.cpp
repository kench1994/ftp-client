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

void command_parser::process(const string & source_line,
                             string & command,
                             vector<string> & args)
{
    istringstream iss(source_line);
    string arg;

    iss >> command;

    while (iss >> quoted(arg))
    {
        args.push_back(arg);
    }
}

} // namespace ftp
