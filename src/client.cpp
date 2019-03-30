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
#include "tools.hpp"
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

string client::open(const string & hostname, const string & port)
{
    control_connection_ = make_unique<control_connection>(hostname, port);
    return control_connection_->read();
}

bool client::is_open() const
{
    return control_connection_ != nullptr;
}

string client::user(const string & username)
{
    control_connection_->write(command::remote::user + " " + username);
    return control_connection_->read();
}

string client::pass(const string & password)
{
    control_connection_->write(command::remote::password + " " + password);
    return control_connection_->read();
}

string client::cd(const std::string & remote_directory)
{
    control_connection_->write(command::remote::cd + " " + remote_directory);
    return control_connection_->read();
}

string client::list(const optional<string> & remote_directory)
{
    string command = command::remote::ls;

    if (remote_directory)
    {
        command += " " + remote_directory.value();
    }

    string multiline_reply;
    string reply_line;

    reply_line = pasv();
    tools::add_line(multiline_reply, reply_line);

    // Minimize lifetime of data_connection.
    {
        boost::asio::ip::tcp::endpoint ep = parse_pasv_reply(reply_line);
        data_connection data_connection(ep);
        data_connection.connect();

        control_connection_->write(command);

        reply_line = control_connection_->read();
        tools::add_line(multiline_reply, reply_line);

        reply_line = data_connection.read();
        tools::add_line(multiline_reply, reply_line);
    }

    reply_line = control_connection_->read();
    tools::add_line(multiline_reply, reply_line);

    return multiline_reply;
}

string client::get(const string & remote_path, ofstream & file)
{
    string multiline_reply;
    string reply_line;

    reply_line = pasv();
    tools::add_line(multiline_reply, reply_line);

    // Minimize lifetime of data_connection.
    {
        boost::asio::ip::tcp::endpoint ep = parse_pasv_reply(reply_line);
        data_connection data_connection(ep);
        data_connection.connect();

        control_connection_->write(command::remote::binary);
        reply_line = control_connection_->read();
        tools::add_line(multiline_reply, reply_line);

        control_connection_->write(command::remote::get + " " + remote_path);
        reply_line = control_connection_->read();
        tools::add_line(multiline_reply, reply_line);

        data_connection.read_file(file);
    }

    reply_line = control_connection_->read();
    tools::add_line(multiline_reply, reply_line);

    return multiline_reply;
}

string client::pwd()
{
    control_connection_->write(command::remote::pwd);
    return control_connection_->read();
}

string client::mkdir(const string & pathname)
{
    control_connection_->write(command::remote::mkdir + " " + pathname);
    return control_connection_->read();
}

string client::binary()
{
    control_connection_->write(command::remote::binary);
    return control_connection_->read();
}

string client::syst()
{
    control_connection_->write(command::remote::syst);
    return control_connection_->read();
}

string client::close()
{
    control_connection_->write(command::remote::close);
    string reply = control_connection_->read();
    control_connection_.reset();
    return reply;
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
