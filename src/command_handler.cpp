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
#include "data_connection.hpp"
#include "negative_completion_code.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

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
            user();
        }
        else if (command == command::local::user)
        {
            user(arguments);
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
        control_connection_.reset();
        cout << ex.what() << endl;
    }
}

bool command_handler::is_needed_connection(const string & command) const
{
    return command == command::local::close || command == command::local::ls ||
           command == command::local::user;
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * This address information is broken into 8-bit fields and the
 * value of each field is transmitted as a decimal number (in
 * character string representation).  The fields are separated
 * by commas.
 *
 * 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
 *
 * Where h1 is the high order 8 bits of the internet host address.
 */
boost::asio::ip::tcp::endpoint
command_handler::parse_pasv_reply(const string & reply)
{
    size_t begin = reply.find('(');
    if (begin == string::npos)
    {
        throw local_exception("Invalid reply of the PASV command.");
    }

    size_t end = reply.find(')');
    if (end == string::npos)
    {
        throw local_exception("Invalid reply of the PASV command.");
    }

    // 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2) -> h1,h2,h3,h4,p1,p2
    string ip_port = reply.substr(begin + 1, end - begin - 1);

    vector<string> tokens;
    boost::split(tokens, ip_port, boost::is_any_of(","));

    static size_t tokens_count = 6;
    if (tokens.size() != tokens_count)
    {
        throw local_exception("Invalid reply of the PASV command.");
    }

    string ip = tokens[0] + '.' + tokens[1] + '.' + tokens[2] + '.' + tokens[3];
    uint16_t port = (boost::lexical_cast<uint16_t>(tokens[4]) * 256) +
                     boost::lexical_cast<uint16_t>(tokens[5]);

    return boost::asio::ip::tcp::endpoint(
             boost::asio::ip::address::from_string(ip), port);
}

void command_handler::open(const vector<string> & arguments)
{
    string hostname;
    static string ftp_port = "21";

    if (arguments.empty())
    {
        hostname = utils::read_line("hostname: ");
    }
    else if (arguments.size() == 1)
    {
        hostname = arguments[0];
    }
    else if (arguments.size() == 2)
    {
        hostname = arguments[0];
        ftp_port = arguments[1];
    }
    else
    {
        throw local_exception("Usage: open <hostname> <port>");
    }

    if (control_connection_)
    {
        throw local_exception("Already connected, use close first.");
    }

    control_connection_ = make_unique<control_connection>(hostname, ftp_port);
    cout << control_connection_->read() << endl;
}

void command_handler::user(const vector<string> & arguments)
{
    string username;

    if (arguments.empty())
    {
        username = utils::read_line("Name: ");
    }
    else if (arguments.size() == 1)
    {
        username = arguments[0];
    }
    else
    {
        throw local_exception("Usage: user <username>");
    }

    control_connection_->write(command::remote::user + " " + username);
    cout << control_connection_->read() << endl;

    /**
     * Send password command.
     *
     * RFC 959: https://tools.ietf.org/html/rfc959
     *
     * This command must be immediately preceded by the
     * user name command, and, for some sites, completes the user's
     * identification for access control.
     */
    string password = utils::read_secure_line("Password: ");
    control_connection_->write(command::remote::password + " " + password);
    cout << control_connection_->read() << endl;
}

void command_handler::ls(const std::vector<std::string> & arguments)
{
    string command;

    if (arguments.empty())
    {
        command = command::remote::ls;
    }
    else if (arguments.size() == 1)
    {
        command = command::remote::ls + " " + arguments[0];
    }
    else
    {
        throw local_exception("Usage: ls <remote-directory>");
    }

    string reply = pasv();
    boost::asio::ip::tcp::endpoint endpoint = parse_pasv_reply(reply);

    // Minimize lifetime of data_connection.
    {
        data_connection data_connection(endpoint);
        data_connection.connect();

        control_connection_->write(command);
        cout << control_connection_->read() << endl;

        cout << data_connection.read();
    }

    cout << control_connection_->read() << endl;
}

string command_handler::pasv()
{
    control_connection_->write(command::remote::pasv);
    string reply = control_connection_->read();

    cout << reply << endl;

    return reply;
}

void command_handler::close()
{
    control_connection_->write(command::remote::close);
    cout << control_connection_->read() << endl;
    control_connection_.reset();
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
    if (control_connection_)
    {
        close();
    }
}

} // namespace ftp
