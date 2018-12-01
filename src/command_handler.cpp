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

void command_handler::execute(const user_command & command)
{
    if (is_needed_connection(command) && !control_connection_)
    {
        throw local_exception(error::not_connected);
    }

    if (command == command::local::open)
    {
        open(command.parameters());
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

bool command_handler::is_needed_connection(const user_command & command) const
{
    return command == command::local::close;
}

void command_handler::open(const vector<string> & parameters)
{
    if (parameters.size() != 2)
    {
        throw local_exception(usage::open);
    }

    if (control_connection_)
    {
        throw local_exception(error::already_connected);
    }

    const string & hostname = parameters[0];
    const string & port = parameters[1];
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
