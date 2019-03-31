/**
 * client.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "client.hpp"
#include "resources.hpp"
#include "local_exception.hpp"
#include <iostream>
#include "data_connection.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace ftp
{

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::make_unique;
using std::optional;
using std::ofstream;

void client::open(const string & hostname, const string & port)
{
    control_connection_ = make_unique<control_connection>(hostname, port);
    cout << control_connection_->read() << endl;
}

bool client::is_open() const
{
    return control_connection_ != nullptr;
}

void client::user(const string & username)
{
    control_connection_->write(command::remote::user + " " + username);
    cout << control_connection_->read() << endl;
}

void client::pass(const string & password)
{
    control_connection_->write(command::remote::password + " " + password);
    cout << control_connection_->read() << endl;
}

void client::cd(const string & remote_directory)
{
    control_connection_->write(command::remote::cd + " " + remote_directory);
    cout << control_connection_->read() << endl;
}

void client::list(const optional<string> & remote_directory)
{
    string command = command::remote::ls;

    if (remote_directory)
    {
        command += " " + remote_directory.value();
    }

    string reply_line;

    reply_line = pasv();
    cout << reply_line << endl;

    // Minimize lifetime of data_connection.
    {
        boost::asio::ip::tcp::endpoint ep = parse_pasv_reply(reply_line);
        data_connection data_connection(ep);
        data_connection.connect();

        control_connection_->write(command);
        cout <<  control_connection_->read() << endl;

        cout << data_connection.read() << endl;
    }

    cout << control_connection_->read() << endl;
}

void client::get(const string & remote_path, ofstream & file)
{
    string reply_line;

    reply_line = pasv();
    cout << reply_line << endl;

    // Minimize lifetime of data_connection.
    {
        boost::asio::ip::tcp::endpoint ep = parse_pasv_reply(reply_line);
        data_connection data_connection(ep);
        data_connection.connect();

        control_connection_->write(command::remote::binary);
        cout << control_connection_->read() << endl;

        control_connection_->write(command::remote::get + " " + remote_path);
        cout << control_connection_->read() << endl;

        data_connection.read_file(file);
    }

    cout << control_connection_->read() << endl;
}

void client::pwd()
{
    control_connection_->write(command::remote::pwd);
    cout << control_connection_->read() << endl;
}

void client::mkdir(const string & pathname)
{
    control_connection_->write(command::remote::mkdir + " " + pathname);
    cout << control_connection_->read() << endl;
}

void client::binary()
{
    control_connection_->write(command::remote::binary);
    cout << control_connection_->read() << endl;
}

void client::syst()
{
    control_connection_->write(command::remote::syst);
    cout << control_connection_->read() << endl;
}

void client::close()
{
    control_connection_->write(command::remote::close);
    cout << control_connection_->read() << endl;
    control_connection_.reset();
}

void client::reset()
{
    control_connection_.reset();
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
client::parse_pasv_reply(const string & reply)
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

string client::pasv()
{
    control_connection_->write(command::remote::pasv);
    return control_connection_->read();
}

} // namespace ftp
