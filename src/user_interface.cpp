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
#include "utils.hpp"
#include <iostream>
#include <vector>

namespace ftp
{

using std::string;
using std::vector;
using std::cout;
using std::endl;

void user_interface::run()
{
    while (true)
    {
        string user_input = utils::read_line(common::ftp_prefix);

        string command = utils::get_command(user_input);
        vector<string> arguments = utils::get_arguments(user_input);

        try
        {
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
