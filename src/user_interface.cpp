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
#include "command_parser.hpp"
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

            command_parser parser(user_input);
            const string & command = parser.get_command();
            const vector<string> & args = parser.get_args();

            command_handler_.execute(command, args);

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
