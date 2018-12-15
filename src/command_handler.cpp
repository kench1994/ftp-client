/**
 * command_handler.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "command_handler.hpp"

using std::string;
using std::vector;

namespace ftp
{

void command_handler::execute(const string & command,
                              const vector<string> & arguments)
{
    // TODO: Move logic of user input handling in this class.
}

} // namespace ftp
