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
#include <memory>

namespace ftp
{

void command_handler::execute(const user_command & command)
{
    if (is_remote_command(command) && !control_connection_)
    {
        throw local_exception(error::not_connected);
    }

    if (command == command::open)
    {
        open(command.parameters());
    }
    else if (command == command::close)
    {
        close();
    }
    else if (command == command::help)
    {
        help();
    }
    else if (command == command::exit)
    {
        exit();
    }
    else
    {
        throw local_exception(error::invalid_command);
    }
}

bool command_handler::is_remote_command(const user_command & command) const
{
    return command == command::close;
}

void command_handler::open(const std::vector<std::string> & parameters)
{
    if (parameters.size() != 2)
    {
        throw local_exception(usage::open);
    }

    if (control_connection_)
    {
        throw local_exception(error::already_connected);
    }

    const std::string & hostname = parameters[0];
    const std::string & port = parameters[1];
    control_connection_ = std::make_unique<control_connection>(hostname, port);
    std::cout << control_connection_->read();

    std::string name = utils::read_line(common::enter_name);
    control_connection_->write(ftp_command::user + " " + name);
    std::cout << control_connection_->read();

    std::string password = utils::read_secure_line(common::enter_password);
    control_connection_->write(ftp_command::password + " " + password);
    std::cout << control_connection_->read();
}

void command_handler::close()
{
    control_connection_->write(ftp_command::close);
    std::cout << control_connection_->read();
    control_connection_.reset();
}

void command_handler::help()
{
    std::cout << common::help;
}

void command_handler::exit()
{
    if (control_connection_)
    {
        close();
    }
}

} // namespace ftp
