/**
 * command_handler.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "command_handler.hpp"
#include "local_exception.hpp"
#include "resources.hpp"
#include "negative_completion_code.hpp"
#include "tools.hpp"
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::optional;

namespace ftp
{

void command_handler::execute(const string & command,
                              const vector<string> & arguments)
{
    if (is_needed_connection(command) && !client_.is_open())
    {
        throw local_exception("Not connected.");
    }

    try
    {
        if (command == command::local::open)
        {
            open(arguments);
            user();
            pass();
        }
        else if (command == command::local::user)
        {
            user(arguments);
            pass();
        }
        else if (command == command::local::close)
        {
            close();
        }
        else if (command == command::local::ls)
        {
            ls(arguments);
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
    catch (const negative_completion_code & ex)
    {
        cout << ex.what() << endl;
    }
    catch (const boost::system::system_error & ex)
    {
        cout << ex.what() << endl;
        client_.reset();
    }
}

bool command_handler::is_needed_connection(const std::string & command) const
{
    return command == command::local::close || command == command::local::ls ||
           command == command::local::user;
}

void command_handler::open(const vector<string> & arguments)
{
    if (arguments.empty())
    {
        string hostname = tools::read_line("hostname: ");
        client_.open(hostname);
    }
    else if (arguments.size() == 1)
    {
        client_.open(arguments[0]);
    }
    else if (arguments.size() == 2)
    {
        client_.open(arguments[0], arguments[1]);
    }
    else
    {
        throw local_exception("Usage: open <hostname> <port>");
    }
}

void command_handler::user()
{
    string username = tools::read_line("username: ");
    client_.user(username);
}

void command_handler::user(const vector<string> & arguments)
{
    if (arguments.empty())
    {
        string username = tools::read_line("username: ");
        client_.user(username);
    }
    if (arguments.size() == 1)
    {
        client_.user(arguments[0]);
    }
    else
    {
        throw local_exception("Usage: user <username>");
    }
}

/**
 * Send password command.
 *
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * This command must be immediately preceded by the
 * user name command, and, for some sites, completes the user's
 * identification for access control.
 */
void command_handler::pass()
{
    string password = tools::read_secure_line("password: ");
    client_.pass(password);
}

void command_handler::close()
{
    client_.close();
}

void command_handler::ls(const vector<string> & arguments)
{
    if (arguments.empty())
    {
        client_.ls();
    }
    else if (arguments.size() == 1)
    {
        client_.ls(arguments[0]);
    }
    else
    {
        throw local_exception("Usage: ls <remote-directory>");
    }
}

void command_handler::help()
{
    cout << "List of FTP commands:\n"
            "\topen <hostname> <port> - Open new connection.\n"
            "\tuser <username> - Send new user information.\n"
            "\tls <remote-directory> - Print list of files in the remote directory.\n"
            "\tclose - Close current connection.\n"
            "\thelp - Print list of FTP commands.\n"
            "\texit - Exit program." << endl;
}

void command_handler::exit()
{
    client_.close();
}

} // namespace ftp
