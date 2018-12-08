/**
 * control_connection.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include "control_connection.hpp"
#include "negative_completion_code.hpp"

namespace ftp
{

using std::string;
using std::istream;

control_connection::control_connection(const string & hostname,
                                       const string & port)
        : socket_(io_context_),
          resolver_(io_context_)
{
    boost::asio::connect(socket_, resolver_.resolve(hostname, port));
}

control_connection::~control_connection()
{
    socket_.shutdown(boost::asio::socket_base::shutdown_both);
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * An FTP reply consists of a three digit number (transmitted as
 * three alphanumeric characters) followed by some text.
 */
string control_connection::get_reply_code(const string & line)
{
    if (line.size() < 3)
    {
        return string();
    }

    return line.substr(0, 3);
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * Thus the format for multi-line replies is that the first line
 * will begin with the exact required reply code, followed
 * immediately by a Hyphen, "-" (also known as Minus), followed by
 * text.
 */
bool control_connection::is_multiline_reply(const string & line) const
{
    if (line.size() < 4)
    {
        return false;
    }

    return line[3] == '-';
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * The last line will begin with the same code, followed
 * immediately by Space <SP>, optionally some text, and the Telnet
 * end-of-line code.
 */
bool control_connection::is_end_of_multiline_reply(const string & line,
                                                   const string & first_reply_code)
{
    string current_reply_code = get_reply_code(line);
    return current_reply_code == first_reply_code && line.size() > 3 && line[3] == ' ';
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * There are five values for the first digit of the reply code:
 *     1yz   Positive Preliminary reply
 *     2yz   Positive Completion reply
 *     3yz   Positive Intermediate reply
 *     4yz   Transient Negative Completion reply
 *     5yz   Permanent Negative Completion reply
 */
bool control_connection::is_negative_completion_code(const std::string & reply) const
{
    return reply[0] == '4' || reply[0] == '5';
}

string control_connection::read()
{
    string reply = read_line();

    if (!is_multiline_reply(reply))
    {
        return reply;
    }

    string multiline_reply = reply;
    string first_reply_code = get_reply_code(reply);

    while (!is_end_of_multiline_reply(reply, first_reply_code))
    {
        reply = read_line();
        multiline_reply += reply;
    }

    return multiline_reply;
}

string control_connection::read_line()
{
    boost::asio::read_until(socket_, read_buf_, '\n');
    istream is(&read_buf_);

    string line;
    getline(is, line);

    if (is_negative_completion_code(line))
    {
        throw negative_completion_code(line);
    }

    return line + "\n";
}

void control_connection::write(const string & command)
{
    boost::asio::write(socket_, boost::asio::buffer(command + "\r\n"));
}

} // namespace ftp
