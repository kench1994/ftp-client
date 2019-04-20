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

namespace ftp
{

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::optional;
using std::ofstream;
using std::ios_base;

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
        else if (command == command::local::ascii)
        {
            ascii();
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
            throw local_exception("%1%: invalid command", command);
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
           command == command::local::ascii ||
           command == command::local::binary ||
           command == command::local::size;
}

void command_handler::open(const vector<string> & arguments)
{
    if (client_.is_open())
    {
        throw local_exception("Already connected, use close first.");
    }

    string hostname;
    string port = "21";

    if (arguments.empty())
    {
        hostname = tools::read_line("hostname: ");
    }
    else if (arguments.size() == 1)
    {
        hostname = arguments[0];
    }
    else if (arguments.size() == 2)
    {
        hostname = arguments[0];
        port = arguments[1];
    }
    else
    {
        throw local_exception("usage: open hostname [ port ]");
    }

    client_.open(hostname, port);
}

void command_handler::user()
{
    string username = tools::read_line("username: ");
    client_.user(username);
}

void command_handler::user(const vector<string> & arguments)
{
    string username;

    if (arguments.empty())
    {
        username = tools::read_line("username: ");
    }
    else if (arguments.size() == 1)
    {
        username = arguments[0];
    }
    else
    {
        throw local_exception("usage: user username");
    }

    client_.user(username);
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
    string password = tools::read_hidden_line("password: ");
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
        throw local_exception("usage: cd remote-directory");
    }

    client_.cd(remote_directory);
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
        throw local_exception("usage: ls [ remote-directory ]");
    }
}

void command_handler::get(const vector<string> & arguments)
{
    string remote_file, local_file;

    if (arguments.empty())
    {
        remote_file = tools::read_line("remote-file: ");
        local_file = tools::get_filename(remote_file);
    }
    else if (arguments.size() == 1)
    {
        remote_file = arguments[0];
        local_file = tools::get_filename(remote_file);
    }
    else if (arguments.size() == 2)
    {
        remote_file = arguments[0];
        local_file = arguments[1];
    }
    else
    {
        throw local_exception("usage: get remote-file [ local-filename ]");
    }

    ofstream file(local_file, ios_base::binary);

    if (!file)
    {
        throw local_exception("Can not create file: " + local_file);
    }

    client_.get(remote_file, file);
}

void command_handler::pwd()
{
    client_.pwd();
}

void command_handler::mkdir(const vector<string> & arguments)
{
    string directory_name;

    if (arguments.empty())
    {
        directory_name = tools::read_line("directory-name: ");
    }
    else if (arguments.size() == 1)
    {
        directory_name = arguments[0];
    }
    else
    {
        throw local_exception("usage: mkdir directory-name");
    }

    client_.mkdir(directory_name);
}

void command_handler::ascii()
{
    client_.ascii();
}

void command_handler::binary()
{
    client_.binary();
}

void command_handler::size(const vector<string> & arguments)
{
    string remote_file;

    if (arguments.empty())
    {
        remote_file = tools::read_line("remote-file: ");
    }
    else if (arguments.size() == 1)
    {
        remote_file = arguments[0];
    }
    else
    {
        throw local_exception("usage: size remote-file");
    }

    client_.size(remote_file);
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
            "\topen hostname [ port ] - Open new connection.\n"
            "\tuser username - Send new user information.\n"
            "\tcd remote-directory - Change remote working directory.\n"
            "\tls [ remote-directory ] - Print list of files in the remote directory.\n"
            "\tget remote-file [ local-file ] - Retrieve a copy of the file.\n"
            "\tpwd - Print the current working directory name.\n"
            "\tmkdir directory-name - Make a directory on the remote machine.\n"
            "\tascii - Set ascii transfer type.\n"
            "\tbinary - Set binary transfer type.\n"
            "\tsize remote-file - Show size of remote file.\n"
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
