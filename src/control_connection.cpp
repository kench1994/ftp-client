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
#include "ftp_exception.hpp"

namespace ftp
{

using std::string;
using std::istream;

control_connection::control_connection(boost::asio::io_context & io_context,
                                       const string & hostname,
                                       const string & port)
        : socket_(io_context),
          resolver_(io_context)
{
    boost::asio::connect(socket_, resolver_.resolve(hostname, port));
}

control_connection::~control_connection()
{
    socket_.shutdown(boost::asio::socket_base::shutdown_both);
}

string control_connection::read()
{
    string reply = read_line();

    if (!is_multiline_reply(reply))
    {
        return reply;
    }

    string first_reply = reply;
    string multiline_reply = reply;

    for (;;)
    {
        reply = read_line();
        multiline_reply += "\n" + reply;

        if (is_end_of_multiline_reply(first_reply, reply))
            break;
    }

    return multiline_reply;
}

void control_connection::write(const string & command)
{
    boost::asio::write(socket_, boost::asio::buffer(command + "\r\n"));
}

string control_connection::read_line()
{
    boost::asio::read_until(socket_, stream_buffer_, "\r\n");
    istream is(&stream_buffer_);

    string line;
    getline(is, line);

    // Remove last '\r' character.
    line.pop_back();

    /**
     * RFC 959: https://tools.ietf.org/html/rfc959
     *
     * A reply is defined to contain the 3-digit code, followed by Space
     * <SP>, followed by one line of text (where some maximum line length
     * has been specified), and terminated by the Telnet end-of-line code.
     *
     * Make sure the line contains at least 3-digit code and followed Space <SP>.
     */
    if (line.size() < 4)
    {
        throw ftp_exception("Invalid server reply: " + line);
    }

    if (is_negative_completion_code(line))
    {
        throw negative_completion_code(line);
    }

    return line;
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * Thus the format for multi-line replies is that the first line
 * will begin with the exact required reply code, followed
 * immediately by a Hyphen, "-" (also known as Minus), followed by
 * text.
 */
bool control_connection::is_multiline_reply(const string & reply) const
{
    return reply[3] == '-';
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * An FTP reply consists of a three digit number (transmitted as
 * three alphanumeric characters) followed by some text.
 *
 * ...
 *
 * The last line will begin with the same code, followed
 * immediately by Space <SP>, optionally some text, and the Telnet
 * end-of-line code.
 */
bool control_connection::is_end_of_multiline_reply(const string & first_reply,
                                                   const string & current_reply) const
{
    return equal(first_reply.cbegin(), first_reply.cbegin() +  3,
                 current_reply.cbegin()) && current_reply[3] == ' ';
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
bool control_connection::is_negative_completion_code(const string & reply) const
{
    return reply[0] == '4' || reply[0] == '5';
}

} // namespace ftp
