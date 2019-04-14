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
#include <boost/algorithm/string.hpp>

namespace ftp
{

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::istringstream;

void user_interface::run()
{
    for (;;)
    {
        try
        {
            string user_input = tools::read_not_empty_line("ftp> ");
            string command = parse_command(user_input);
            vector<string> arguments = parse_arguments(user_input);

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

string user_interface::parse_command(const string & user_input)
{
    string command;

    istringstream iss(user_input);
    iss >> command;

    return command;
}

vector<string> user_interface::parse_arguments(const string & user_input)
{
    vector<string> arguments;
    boost::split(arguments, user_input, boost::is_any_of(" "));

    if (!arguments.empty())
    {
        arguments.erase(arguments.begin());
    }

    return arguments;
}

} // namespace ftp
