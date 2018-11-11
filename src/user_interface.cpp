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

        string command;
        getline(cin, command);

        vector<string> parsed_command = command_parser::parse(command);
        if (parsed_command.empty())
        {
            continue;
        }

        try
        {
            command_handler_.execute(parsed_command);
        }
        catch (const std::exception & ex)
        {
            cout << ex.what() << endl;
        }

        const string & user_command = parsed_command[0];
        if (user_command == user_command::exit)
        {
            break;
        }
    }
}

} // namespace ftp
