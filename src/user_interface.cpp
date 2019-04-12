/**
 * user_interface.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "user_interface.hpp"
#include "resources.hpp"
#include "local_exception.hpp"
#include "tools.hpp"
#include <iostream>

namespace ftp
{

using std::string;
using std::vector;
using std::cout;
using std::endl;

void user_interface::run()
{
    for (;;)
    {
        try
        {
            string user_input = tools::read_line("ftp> ");
            string command = tools::get_command(user_input);
            vector<string> arguments = tools::get_arguments(user_input);

            command_handler_.execute(command, arguments);

            if (command == command::local::exit)
            {
                return;
            }
        }
        catch (const local_exception & ex)
        {
            cout << ex.what() << endl;
        }
    }
}

} // namespace ftp
