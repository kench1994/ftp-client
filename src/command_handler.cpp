/**
 * command_handler.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "command_handler.hpp"
#include "resources.hpp"
#include <iostream>

using std::cout;
using std::endl;

namespace ftp
{

void command_handler::execute(const user_command & command)
{
    if (is_local_command(command))
    {
        execute_local_command(command);
    }
    else if (is_remote_command(command))
    {
        execute_remote_command(command);
    }
    else
    {
        throw std::runtime_error(error::invalid_command);
    }
}

bool command_handler::is_local_command(const user_command & command) const
{
    return command == command::open ||
           command == command::help ||
           command == command::exit;
}

bool command_handler::is_remote_command(const user_command & command) const
{
    return command == command::close;
}

void command_handler::execute_local_command(const user_command & command)
{
    if (command == command::open)
    {
        open(command.parameters());
    }
    else if (command == command::help)
    {
        help();
    }
    else if (command == command::exit)
    {
        exit();
    }
}

void command_handler::execute_remote_command(const user_command & command)
{
    if (!session_.control_connection_is_open())
    {
        throw std::runtime_error(error::not_connected);
    }

    if (command == command::close)
    {
        close();
    }
}

void command_handler::open(const vector<string> & parameters)
{
    if (parameters.size() != 2)
    {
        throw std::runtime_error(usage::open);
    }

    if (session_.control_connection_is_open())
    {
        throw std::runtime_error(error::already_connected);
    }

    const string & hostname = parameters[0];
    const string & port = parameters[1];
    session_.open_control_connection(hostname, port);
    cout << session_.read_control_connection();
}

void command_handler::close()
{
    session_.write_control_connection(ftp_command::close);
    cout << session_.read_control_connection();
    session_.close_control_connection();
}

void command_handler::help()
{
    cout << common::help;
}

void command_handler::exit()
{
    if (session_.control_connection_is_open())
    {
        close();
    }
}

} // namespace ftp
