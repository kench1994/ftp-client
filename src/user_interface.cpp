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

string user_interface::read_line(const string & greeting)
{
    string line;

    while (line.empty())
    {
        cout << greeting;
        getline(cin, line);
    }

    return line;
}

void user_interface::run()
{
    while (true)
    {
        string user_input = read_line(common::ftp_prefix);

        try
        {
            user_command command = command_parser::parse(user_input);
            command_handler_.execute(command);

            if (command == command::exit)
            {
                return;
            }
        }
        catch (const std::exception & ex)
        {
            cout << ex.what() << endl;
        }
    }
}

} // namespace ftp
