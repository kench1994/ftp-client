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
#include "negative_completion_code.hpp"
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;

namespace ftp
{

void command_handler::execute(const string & command,
                              const vector<string> & arguments)
{
    if (is_needed_connection(command) && !client_.is_open())
    {
        throw local_exception("Not connected.");
    }

    try
    {
        // TODO: add handling of commands.
        if (command == command::local::open)
        {

        }
        else if (command == command::local::user)
        {

        }
        else if (command == command::local::close)
        {

        }
        else if (command == command::local::ls)
        {

        }
        else if (command == command::local::help)
        {

        }
        else if (command == command::local::exit)
        {

        }
        else
        {
            throw local_exception("Invalid command. "
                                  "Use 'help' to display list of FTP commands.");
        }
    }
    catch (const negative_completion_code & ex)
    {
        cout << ex.what() << endl;
    }
    catch (const boost::system::system_error & ex)
    {
        cout << ex.what() << endl;
        client_.reset();
    }
}

bool command_handler::is_needed_connection(const std::string & command) const
{
    return command == command::local::close || command == command::local::ls ||
           command == command::local::user;
}

} // namespace ftp
