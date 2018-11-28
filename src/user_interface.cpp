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
#include "local_exception.hpp"
#include "utils.hpp"
#include <iostream>

namespace ftp
{

void user_interface::run()
{
    while (true)
    {
        std::string user_input = utils::read_line(common::ftp_prefix);

        try
        {
            user_command command = command_parser::parse(user_input);
            command_handler_.execute(command);

            if (command == command::exit)
            {
                return;
            }
        }
        catch (const local_exception & ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }
}

} // namespace ftp
