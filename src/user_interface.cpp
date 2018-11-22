/**
 * user_interface.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "user_interface.hpp"
#include "command_parser.hpp"
#include "resources.hpp"
#include <iostream>

using std::cout;
using std::cin;
using std::endl;
using std::string;

namespace ftp
{

void user_interface::run()
{
    while (true)
    {
        cout << common::ftp_prefix;

        string user_input;
        getline(cin, user_input);

        if (user_input.empty())
            continue;

        user_command command;
        try
        {
            command = command_parser::parse(user_input);
            command_handler_.execute(command);
        }
        catch (const std::exception & ex)
        {
            cout << ex.what() << endl;
        }

        if (command == command::exit)
        {
            return;
        }
    }
}

} // namespace ftp
