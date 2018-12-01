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
using std::endl;
using std::make_unique;

void command_handler::execute(const string & command,
                              const vector<string> & arguments)
{
    if (is_needed_connection(command) && !control_connection_)
    {
        throw local_exception("Not connected.");
    }

    try
    {
        if (command == command::local::open)
        {
            open(arguments);
            login();
        }
        else if (command == command::local::user)
        {
            login(arguments);
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
            throw local_exception("Invalid command. "
                                  "Use 'help' to display list of FTP commands.");
        }
    }
    catch (const boost::system::system_error & ex)
    {
        control_connection_.reset();
        cout << ex.what() << endl;
    }
}

bool command_handler::is_needed_connection(const string & command) const
{
    return command == command::local::close;
}

void command_handler::login()
{
    string username = utils::read_line("Name: ");
    login(username);
}

void command_handler::login(const string & username)
{
    user(username);
    string password = utils::read_secure_line("Password: ");
    pass(password);
}

void command_handler::login(const vector<string> & arguments)
{
    if (arguments.empty())
    {
        login();
    }
    else if (arguments.size() == 1)
    {
        login(arguments[0]);
    }
    else
    {
        throw local_exception("Usage: user <username>");
    }
}

void command_handler::open(const vector<string> & arguments)
{
    if (arguments.size() != 2)
    {
        throw local_exception("Usage: open <hostname> <port>");
    }

    if (control_connection_)
    {
        throw local_exception("Already connected, use close first.");
    }

    control_connection_ = make_unique<control_connection>(arguments[0], arguments[1]);
    cout << control_connection_->read();
}

void command_handler::user(const string & username)
{
    control_connection_->write(command::remote::user + " " + username);
    cout << control_connection_->read();
}

void command_handler::pass(const string & password)
{
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
    cout << "List of FTP commands:\n"
            "\topen <hostname> <open> - Open new connection.\n"
            "\tclose - Close current connection.\n"
            "\thelp - Print list of FTP commands.\n"
            "\texit - Exit program." << endl;
}

void command_handler::exit()
{
    if (control_connection_)
    {
        close();
    }
}

} // namespace ftp
