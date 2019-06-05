/*
 * MIT License
 *
 * Copyright (c) 2019 Denis Kovalchuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "command_handler.hpp"
#include "local_exception.hpp"
#include "resources.hpp"
#include "tools.hpp"
#include "ftp/ftp_exception.hpp"
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::optional;
using std::ofstream;
using std::ios_base;
using ftp::ftp_exception;

void command_handler::execute(const string & command,
                              const vector<string> & args)
{
    if (is_needed_connection(command) && !client_.is_open())
    {
        throw local_exception("Not connected.");
    }

    try
    {
        if (command == command::local::open)
        {
            open(args);
            user();
            pass();
            // Use binary mode to transfer files by default.
            binary();
        }
        else if (command == command::local::user)
        {
            user(args);
            pass();
        }
        else if (command == command::local::close)
        {
            close();
        }
        else if (command == command::local::cd)
        {
            cd(args);
        }
        else if (command == command::local::ls)
        {
            ls(args);
        }
        else if (command == command::local::get)
        {
            get(args);
        }
        else if (command == command::local::pwd)
        {
            pwd();
        }
        else if (command == command::local::mkdir)
        {
            mkdir(args);
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
            size(args);
        }
        else if (command == command::local::noop)
        {
            noop();
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
    catch (const ftp_exception & ex)
    {
        cout << ex.what() << endl;
        client_.reset();
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
           command == command::local::size ||
           command == command::local::noop;
}

void command_handler::open(const vector<string> & args)
{
    if (client_.is_open())
    {
        throw local_exception("Already connected, use close first.");
    }

    string hostname;
    string port = "21";

    if (args.empty())
    {
        hostname = tools::read_line("hostname: ");
    }
    else if (args.size() == 1)
    {
        hostname = args[0];
    }
    else if (args.size() == 2)
    {
        hostname = args[0];
        port = args[1];
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

void command_handler::user(const vector<string> & args)
{
    string username;

    if (args.empty())
    {
        username = tools::read_line("username: ");
    }
    else if (args.size() == 1)
    {
        username = args[0];
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

void command_handler::cd(const vector<string> & args)
{
    string remote_directory;

    if (args.empty())
    {
        remote_directory = tools::read_line("remote directory: ");
    }
    else if (args.size() == 1)
    {
        remote_directory = args[0];
    }
    else
    {
        throw local_exception("usage: cd remote-directory");
    }

    client_.cd(remote_directory);
}

void command_handler::ls(const vector<string> & args)
{
    if (args.empty())
    {
        client_.ls();
    }
    else if (args.size() == 1)
    {
        client_.ls(args[0]);
    }
    else
    {
        throw local_exception("usage: ls [ remote-directory ]");
    }
}

void command_handler::get(const vector<string> & args)
{
    string remote_file, local_file;

    if (args.empty())
    {
        remote_file = tools::read_line("remote-file: ");
        local_file = tools::get_filename(remote_file);
    }
    else if (args.size() == 1)
    {
        remote_file = args[0];
        local_file = tools::get_filename(remote_file);
    }
    else if (args.size() == 2)
    {
        remote_file = args[0];
        local_file = args[1];
    }
    else
    {
        throw local_exception("usage: get remote-file [ local-filename ]");
    }

    ofstream file(local_file, ios_base::binary);

    if (!file)
    {
        throw local_exception("%1%: can't create file", local_file);
    }

    client_.get(remote_file, file);
}

void command_handler::pwd()
{
    client_.pwd();
}

void command_handler::mkdir(const vector<string> & args)
{
    string directory_name;

    if (args.empty())
    {
        directory_name = tools::read_line("directory-name: ");
    }
    else if (args.size() == 1)
    {
        directory_name = args[0];
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

void command_handler::size(const vector<string> & args)
{
    string remote_file;

    if (args.empty())
    {
        remote_file = tools::read_line("remote-file: ");
    }
    else if (args.size() == 1)
    {
        remote_file = args[0];
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

void command_handler::noop()
{
    client_.noop();
}

void command_handler::close()
{
    client_.close();
}

void command_handler::help()
{
    cout << "list of ftp commands:\n"
            "\topen hostname [ port ] - open new connection\n"
            "\tuser username - send new user information\n"
            "\tcd remote-directory - change remote working directory\n"
            "\tls [ remote-directory ] - print list of files in the remote directory\n"
            "\tget remote-file [ local-file ] - retrieve a copy of the file\n"
            "\tpwd - print the current working directory name\n"
            "\tmkdir directory-name - make a directory on the remote machine\n"
            "\tascii - set ascii transfer type\n"
            "\tbinary - set binary transfer type\n"
            "\tsize remote-file - show size of remote file\n"
            "\tsyst - show remote system type\n"
            "\tnoop - no operation\n"
            "\tclose - close current connection\n"
            "\thelp - print list of ftp commands\n"
            "\texit - exit program" << endl;
}

void command_handler::exit()
{
    if (client_.is_open())
    {
        client_.close();
    }
}
