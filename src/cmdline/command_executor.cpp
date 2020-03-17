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

#include "command_executor.hpp"
#include "local_exception.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <boost/lexical_cast/try_lexical_convert.hpp>

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::optional;

command_executor::command_executor()
{
    client_.subscribe(&stdout_writer_);
}

void command_executor::execute(command command, const vector<string> & args)
{
    if (is_needed_connection(command) && !client_.is_open())
    {
        throw local_exception("Not connected.");
    }

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
    else if (command == command::put)
    {
        put(args);
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
    else if (command == command::rmdir)
    {
        rmdir(args);
    }
    else if (command == command::del)
    {
        del(args);
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

bool command_executor::is_needed_connection(command command)
{
    switch (command)
    {
    case command::user:
    case command::cd:
    case command::ls:
    case command::put:
    case command::get:
    case command::pwd:
    case command::mkdir:
    case command::rmdir:
    case command::del:
    case command::stat:
    case command::syst:
    case command::binary:
    case command::size:
    case command::noop:
    case command::close:
        return true;
    case command::open:
    case command::help:
    case command::exit:
        return false;
    default:
        assert(false);
        return false;
    }
}

void command_executor::open(const vector<string> & args)
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
    string password = utils::read_password("password: ");

    client_.user(username, password);
}

void command_executor::user(const vector<string> & args)
{
    string username;
    string password;

    if (args.empty())
    {
        username = utils::read_line("username: ");
        password = utils::read_password("password: ");
    }
    else if (args.size() == 1)
    {
        username = args[0];
        password = utils::read_password("password: ");
    }
    else
    {
        throw local_exception("usage: user username");
    }

    client_.user(username, password);
}

void command_executor::cd(const vector<string> & args)
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

void command_executor::ls(const vector<string> & args)
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

void command_executor::put(const vector<string> & args)
{
    string local_file, remote_file;

    if (args.empty())
    {
        local_file = utils::read_line("local-file: ");
        remote_file = utils::get_filename(local_file);
    }
    else if (args.size() == 1)
    {
        local_file = args[0];
        remote_file = utils::get_filename(local_file);
    }
    else if (args.size() == 2)
    {
        local_file = args[0];
        remote_file = args[1];
    }
    else
    {
        throw local_exception("usage: put local-file [ remote-file ]");
    }

    client_.upload(local_file, remote_file);
}

void command_executor::get(const vector<string> & args)
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

    client_.download(remote_file, local_file);
}

void command_executor::pwd()
{
    client_.pwd();
}

void command_executor::mkdir(const vector<string> & args)
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

void command_executor::rmdir(const vector<string> & args)
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
        throw local_exception("usage: rmdir directory-name");
    }

    client_.rmdir(directory_name);
}

void command_executor::del(const vector<string> & args)
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
        throw local_exception("usage: del remote-file");
    }

    client_.rm(remote_file);
}

void command_executor::binary()
{
    client_.binary();
}

void command_executor::size(const vector<string> & args)
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

void command_executor::stat(const vector<string> & args)
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

void command_executor::syst()
{
    client_.system();
}

void command_executor::noop()
{
    client_.noop();
}

void command_executor::close()
{
    client_.close();
}

void command_executor::help()
{
    cout <<
        "List of ftp commands:\n"
        "  open hostname [ port ] - open new connection\n"
        "  user username - send new user information\n"
        "  cd remote-directory - change remote working directory\n"
        "  ls [ remote-directory ] - print list of files in the remote directory\n"
        "  put local-file [ remote-file ] - store a file at the server\n"
        "  get remote-file [ local-file ] - retrieve a copy of the file\n"
        "  pwd - print the current working directory name\n"
        "  mkdir directory-name - make a directory on the remote machine\n"
        "  rmdir directory-name - remove a directory\n"
        "  del remote-file - delete a file\n"
        "  binary - set binary transfer type\n"
        "  size remote-file - show size of remote file\n"
        "  stat [ remote-file ] - print server information\n"
        "  syst - show remote system type\n"
        "  noop - no operation\n"
        "  close - close current connection\n"
        "  help - print list of ftp commands\n"
        "  exit - exit program\n";
}

void command_executor::exit()
{
    if (client_.is_open())
    {
        client_.close();
    }
}
