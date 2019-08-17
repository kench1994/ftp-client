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

#include "commands_handler.hpp"
#include "local_exception.hpp"
#include "utils.hpp"
#include "ftp/ftp_exception.hpp"
#include <iostream>
#include <boost/lexical_cast/try_lexical_convert.hpp>

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::optional;
using ftp::ftp_exception;

commands_handler::commands_handler()
{
    client_.subscribe(&stdout_writer_);
}

void commands_handler::execute(command command, const vector<string> & args)
{
    if (is_needed_connection(command) && !client_.is_open())
    {
        throw local_exception("Not connected.");
    }

    try
    {
        if (command == command::open)
        {
            open(args);
            // Use binary mode to transfer files by default.
            binary();
        }
        else if (command == command::user)
        {
            user(args);
            // Use binary mode to transfer files by default.
            binary();
        }
        else if (command == command::close)
        {
            close();
        }
        else if (command == command::cd)
        {
            cd(args);
        }
        else if (command == command::ls)
        {
            ls(args);
        }
        else if (command == command::get)
        {
            get(args);
        }
        else if (command == command::pwd)
        {
            pwd();
        }
        else if (command == command::mkdir)
        {
            mkdir(args);
        }
        else if (command == command::stat)
        {
            stat(args);
        }
        else if (command == command::syst)
        {
            syst();
        }
        else if (command == command::binary)
        {
            binary();
        }
        else if (command == command::size)
        {
            size(args);
        }
        else if (command == command::noop)
        {
            noop();
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
            assert(false);
        }
    }
    catch (const ftp_exception & ex)
    {
        cout << ex.what() << endl;
    }
}

bool commands_handler::is_needed_connection(command command) const
{
    switch (command)
    {
    case command::user:
    case command::cd:
    case command::ls:
    case command::get:
    case command::pwd:
    case command::mkdir:
    case command::stat:
    case command::syst:
    case command::binary:
    case command::size:
    case command::noop:
    case command::close:
        return true;
    default:
        return false;
    }
}

void commands_handler::open(const vector<string> & args)
{
    if (client_.is_open())
    {
        throw local_exception("Already connected, use close first.");
    }

    string hostname;
    uint16_t port = 21;

    if (args.empty())
    {
        hostname = utils::read_line("hostname: ");
    }
    else if (args.size() == 1)
    {
        hostname = args[0];
    }
    else if (args.size() == 2)
    {
        hostname = args[0];

        if (!boost::conversion::try_lexical_convert(args[1], port))
        {
            throw local_exception("Invalid port number.");
        }
    }
    else
    {
        throw local_exception("usage: open hostname [ port ]");
    }

    client_.open(hostname, port);

    string username = utils::read_line("username: ");
    string password = utils::read_hidden_line("password: ");

    client_.login(username, password);
}

void commands_handler::user(const vector<string> & args)
{
    string username;
    string password;

    if (args.empty())
    {
        username = utils::read_line("username: ");
        password = utils::read_hidden_line("password: ");
    }
    else if (args.size() == 1)
    {
        username = args[0];
        password = utils::read_hidden_line("password: ");
    }
    else
    {
        throw local_exception("usage: user username");
    }

    client_.login(username, password);
}

void commands_handler::cd(const vector<string> & args)
{
    string remote_directory;

    if (args.empty())
    {
        remote_directory = utils::read_line("remote directory: ");
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

void commands_handler::ls(const vector<string> & args)
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

void commands_handler::get(const vector<string> & args)
{
    string remote_file, local_file;

    if (args.empty())
    {
        remote_file = utils::read_line("remote-file: ");
        local_file = utils::get_filename(remote_file);
    }
    else if (args.size() == 1)
    {
        remote_file = args[0];
        local_file = utils::get_filename(remote_file);
    }
    else if (args.size() == 2)
    {
        remote_file = args[0];
        local_file = args[1];
    }
    else
    {
        throw local_exception("usage: get remote-file [ local-file ]");
    }

    client_.get(remote_file, local_file);
}

void commands_handler::pwd()
{
    client_.pwd();
}

void commands_handler::mkdir(const vector<string> & args)
{
    string directory_name;

    if (args.empty())
    {
        directory_name = utils::read_line("directory-name: ");
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

void commands_handler::binary()
{
    client_.binary();
}

void commands_handler::size(const vector<string> & args)
{
    string remote_file;

    if (args.empty())
    {
        remote_file = utils::read_line("remote-file: ");
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

void commands_handler::stat(const vector<string> & args)
{
    if (args.empty())
    {
        client_.stat();
    }
    else if (args.size() == 1)
    {
        client_.stat(args[0]);
    }
    else
    {
        throw local_exception("usage: stat [ remote-file ]");
    }
}

void commands_handler::syst()
{
    client_.syst();
}

void commands_handler::noop()
{
    client_.noop();
}

void commands_handler::close()
{
    client_.close();
}

void commands_handler::help()
{
    cout <<
        "List of ftp commands:\n"
        "  open hostname [ port ] - open new connection\n"
        "  user username - send new user information\n"
        "  cd remote-directory - change remote working directory\n"
        "  ls [ remote-directory ] - print list of files in the remote directory\n"
        "  get remote-file [ local-file ] - retrieve a copy of the file\n"
        "  pwd - print the current working directory name\n"
        "  mkdir directory-name - make a directory on the remote machine\n"
        "  binary - set binary transfer type\n"
        "  size remote-file - show size of remote file\n"
        "  stat [ remote-file ] - print server information\n"
        "  syst - show remote system type\n"
        "  noop - no operation\n"
        "  close - close current connection\n"
        "  help - print list of ftp commands\n"
        "  exit - exit program\n";
}

void commands_handler::exit()
{
    if (client_.is_open())
    {
        client_.close();
    }
}
