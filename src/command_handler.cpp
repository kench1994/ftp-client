/**
 * command_handler.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "command_handler.hpp"
#include "resources.hpp"
#include "local_exception.hpp"
#include "utils.hpp"
#include <iostream>

namespace ftp
{

using std::string;
using std::vector;
using std::cout;
using std::make_unique;

void command_handler::execute(const string & command,
                              const vector<string> & arguments)
{
    if (is_needed_connection(command) && !control_connection_)
    {
        throw local_exception(error::not_connected);
    }

    if (command == command::local::open)
    {
        if (arguments.size() != 2)
        {
            throw local_exception("Usage: open <hostname> <port>");
        }

        open(arguments[0], arguments[1]);
    }
    else if (command == command::local::close)
    {
        close();
    }
    else if (command == command::local::help)
    {
        help();
    }
    else if (command == command::local::exit)
    {
        exit();
    }
    else
    {
        throw local_exception(error::invalid_command);
    }
}

bool command_handler::is_needed_connection(const string & command) const
{
    return command == command::local::close;
}

void command_handler::open(const string & hostname, const string & port)
{
    if (control_connection_)
    {
        throw local_exception(error::already_connected);
    }

    control_connection_ = make_unique<control_connection>(hostname, port);
    cout << control_connection_->read();

    string name = utils::read_line(common::enter_name);
    control_connection_->write(command::remote::user + " " + name);
    cout << control_connection_->read();

    string password = utils::read_secure_line(common::enter_password);
    control_connection_->write(command::remote::password + " " + password);
    cout << control_connection_->read();
}

void command_handler::close()
{
    control_connection_->write(command::remote::close);
    cout << control_connection_->read();
    control_connection_.reset();
}

void command_handler::help()
{
    cout << common::help;
}

void command_handler::exit()
{
    if (control_connection_)
    {
        close();
    }
}

} // namespace ftp
