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

void command_handler::execute(const vector<string> & command)
{
    if (command.empty())
    {
        return;
    }

    const string & user_command = command[0];

    if (is_local_command(user_command))
    {
        execute_local_command(command);
    }
    else if (is_remote_command(user_command))
    {
        execute_remote_command(command);
    }
    else
    {
        throw std::runtime_error(error::invalid_command);
    }
}

bool command_handler::is_local_command(const string & command) const
{
    return command == user_command::open ||
           command == user_command::help ||
           command == user_command::exit;
}

bool command_handler::is_remote_command(const string & command) const
{
    return command == user_command::close;
}

void command_handler::execute_local_command(const vector<string> & command)
{
    const string & user_command = command[0];

    if (user_command == user_command::open)
    {
        open(command);
    }
    else if (user_command == user_command::help)
    {
        help();
    }
    else if (user_command == user_command::exit)
    {
        exit();
    }
}

void command_handler::execute_remote_command(const vector<string> & command)
{
    if (!session_.control_connection_is_open())
    {
        throw std::runtime_error(error::not_connected);
    }

    const string & user_command = command[0];

    if (user_command == user_command::close)
    {
        close();
    }
}

void command_handler::open(const vector<string> & command)
{
    if (command.size() != 3)
    {
        throw std::runtime_error(usage::open);
    }

    const string & hostname = command[1];
    const string & port = command[2];
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
