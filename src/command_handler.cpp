/**
 * command_handler.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "command_handler.hpp"
#include "local_exception.hpp"
#include "resources.hpp"

using std::string;
using std::vector;

namespace ftp
{

void command_handler::execute(const string & command,
                              const vector<string> & arguments)
{
    if (is_needed_connection(command) && !client_.is_open())
    {
        throw local_exception("Not connected.");
    }
}

bool command_handler::is_needed_connection(const std::string & command) const
{
    return command == command::local::close || command == command::local::ls ||
           command == command::local::user;
}

} // namespace ftp
