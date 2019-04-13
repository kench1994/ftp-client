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
using std::ofstream;
using std::ios_base;

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
            // Use binary mode to transfer files by default.
            binary();
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
        else if (command == command::local::cd)
        {
            cd(arguments);
        }
        else if (command == command::local::ls)
        {
            ls(arguments);
        }
        else if (command == command::local::get)
        {
            get(arguments);
        }
        else if (command == command::local::pwd)
        {
            pwd();
        }
        else if (command == command::local::mkdir)
        {
            mkdir(arguments);
        }
        else if (command == command::local::syst)
        {
            syst();
        }
        else if (command == command::local::binary)
        {
            binary();
        }
        else if (command == command::local::size)
        {
            size(arguments);
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

bool command_handler::is_needed_connection(const string & command) const
{
    return command == command::local::close ||
           command == command::local::cd ||
           command == command::local::ls ||
           command == command::local::get ||
           command == command::local::user ||
           command == command::local::pwd ||
           command == command::local::mkdir ||
           command == command::local::syst ||
           command == command::local::binary ||
           command == command::local::size;
}

void command_handler::open(const vector<string> & arguments)
{
    if (client_.is_open())
    {
        throw local_exception("Already connected, use close first.");
    }

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
    else if (arguments.size() == 1)
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

void command_handler::cd(const vector<string> & arguments)
{
    string remote_directory;

    if (arguments.empty())
    {
        remote_directory = tools::read_line("remote directory: ");
    }
    else if (arguments.size() == 1)
    {
        remote_directory = arguments[0];
    }
    else
    {
        throw local_exception("Usage: cd <remote-directory>");
    }

    client_.cd(remote_directory);
}

void command_handler::ls(const vector<string> & arguments)
{
    if (arguments.empty())
    {
        client_.list();
    }
    else if (arguments.size() == 1)
    {
        client_.list(arguments[0]);
    }
    else
    {
        throw local_exception("Usage: ls <remote-directory>");
    }
}

void command_handler::get(const vector<string> & arguments)
{
    string remote_path, local_filename;

    if (arguments.empty())
    {
        remote_path = tools::read_line("filename: ");
        local_filename = tools::get_filename(remote_path);
    }
    else if (arguments.size() == 1)
    {
        remote_path = arguments[0];
        local_filename = tools::get_filename(remote_path);
    }
    else if (arguments.size() == 2)
    {
        remote_path = arguments[0];
        local_filename = arguments[1];
    }
    else
    {
        throw local_exception("Usage: get <remote-path> <local-filename>");
    }

    ofstream file(local_filename, ios_base::binary);

    if (!file)
    {
        throw local_exception("Can not create file: " + local_filename);
    }

    client_.get(remote_path, file);
}

void command_handler::pwd()
{
    client_.pwd();
}

void command_handler::mkdir(const vector<string> & arguments)
{
    string pathname;

    if (arguments.empty())
    {
        pathname = tools::read_line("pathname: ");
    }
    else if (arguments.size() == 1)
    {
        pathname = arguments[0];
    }
    else
    {
        throw local_exception("Usage: mkdir <pathname>");
    }

    client_.mkdir(pathname);
}

void command_handler::binary()
{
    client_.binary();
}

void command_handler::size(const vector<string> & arguments)
{
    string filename;

    if (arguments.empty())
    {
        filename = tools::read_line("filename: ");
    }
    else if (arguments.size() == 1)
    {
        filename = arguments[0];
    }
    else
    {
        throw local_exception("Usage: size <filename>");
    }

    client_.size(filename);
}

void command_handler::syst()
{
    client_.syst();
}

void command_handler::close()
{
    client_.close();
}

void command_handler::help()
{
    cout << "List of FTP commands:\n"
            "\topen <hostname> <port> - Open new connection.\n"
            "\tuser <username> - Send new user information.\n"
            "\tcd <remote-directory> - Change remote working directory.\n"
            "\tls <remote-directory> - Print list of files in the remote directory.\n"
            "\tget <remote-path> <local-filename> - Retrieve a copy of the file.\n"
            "\tpwd - Print the current working directory name.\n"
            "\tmkdir <pathname> - Make a directory with the name \"pathname\".\n"
            "\tbinary - Set binary transfer type.\n"
            "\tsize - Show size of remote file.\n"
            "\tsyst - Show remote system type.\n"
            "\tclose - Close current connection.\n"
            "\thelp - Print list of FTP commands.\n"
            "\texit - Exit program." << endl;
}

void command_handler::exit()
{
    if (client_.is_open())
    {
        client_.close();
    }
}

} // namespace ftp
