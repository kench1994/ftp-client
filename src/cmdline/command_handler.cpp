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
#include "cmdline_exception.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <boost/lexical_cast/try_lexical_convert.hpp>

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::optional;

command_handler::command_handler()
{
    ftp_client_.subscribe(&stdout_writer_);
}

void command_handler::handle(command command, const vector<string> & args)
{
    if (command == command::open)
    {
        open(args);
    }
    else if (command == command::user)
    {
        user(args);
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
        throw cmdline_exception("Invalid command.");
    }
}

void command_handler::open(const vector<string> & args)
{
    if (ftp_client_.is_open())
    {
        throw cmdline_exception("Already connected, use close first.");
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
            throw cmdline_exception("Invalid port number.");
        }
    }
    else
    {
        throw cmdline_exception("usage: open hostname [ port ]");
    }

    bool ftp_result = ftp_client_.open(hostname, port);

    if (!ftp_result)
    {
        return;
    }

    string username = utils::read_line("username: ");
    string password = utils::read_password("password: ");

    ftp_result = ftp_client_.login(username, password);

    if (!ftp_result)
    {
        return;
    }

    /* Use binary mode to transfer files by default. */
    ftp_client_.binary();
}

void command_handler::user(const vector<string> & args)
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
        throw cmdline_exception("usage: user username");
    }

    bool ftp_result = ftp_client_.login(username, password);

    if (!ftp_result)
    {
        return;
    }

    /* Use binary mode to transfer files by default. */
    ftp_client_.binary();
}

void command_handler::cd(const vector<string> & args)
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
        throw cmdline_exception("usage: cd remote-directory");
    }

    ftp_client_.cd(remote_directory);
}

void command_handler::ls(const vector<string> & args)
{
    if (args.empty())
    {
        ftp_client_.ls();
    }
    else if (args.size() == 1)
    {
        ftp_client_.ls(args[0]);
    }
    else
    {
        throw cmdline_exception("usage: ls [ remote-directory ]");
    }
}

void command_handler::put(const vector<string> & args)
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
        throw cmdline_exception("usage: put local-file [ remote-file ]");
    }

    ftp_client_.upload(local_file, remote_file);
}

void command_handler::get(const vector<string> & args)
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
        throw cmdline_exception("usage: get remote-file [ local-file ]");
    }

    ftp_client_.download(remote_file, local_file);
}

void command_handler::pwd()
{
    ftp_client_.pwd();
}

void command_handler::mkdir(const vector<string> & args)
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
        throw cmdline_exception("usage: mkdir directory-name");
    }

    ftp_client_.mkdir(directory_name);
}

void command_handler::rmdir(const vector<string> & args)
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
        throw cmdline_exception("usage: rmdir directory-name");
    }

    ftp_client_.rmdir(directory_name);
}

void command_handler::del(const vector<string> & args)
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
        throw cmdline_exception("usage: del remote-file");
    }

    ftp_client_.rm(remote_file);
}

void command_handler::binary()
{
    ftp_client_.binary();
}

void command_handler::size(const vector<string> & args)
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
        throw cmdline_exception("usage: size remote-file");
    }

    ftp_client_.size(remote_file);
}

void command_handler::stat(const vector<string> & args)
{
    if (args.empty())
    {
        ftp_client_.stat();
    }
    else if (args.size() == 1)
    {
        ftp_client_.stat(args[0]);
    }
    else
    {
        throw cmdline_exception("usage: stat [ remote-file ]");
    }
}

void command_handler::syst()
{
    ftp_client_.system();
}

void command_handler::noop()
{
    ftp_client_.noop();
}

void command_handler::close()
{
    ftp_client_.close();
}

void command_handler::help()
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

void command_handler::exit()
{
    if (ftp_client_.is_open())
    {
        ftp_client_.close();
    }
}
