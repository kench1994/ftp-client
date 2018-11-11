/**
 * session.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include "session.hpp"

using std::istream;

namespace ftp
{

session::session()
        : control_connection_(io_context_),
          resolver_(io_context_)
{
}

void session::open_control_connection(const string & hostname, const string & port)
{
    asio::connect(control_connection_, resolver_.resolve(hostname, port));
}

void session::close_control_connection()
{
    control_connection_.close();
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * An FTP reply consists of a three digit number (transmitted as
 * three alphanumeric characters) followed by some text.
 */
string session::get_reply_code(const string & line)
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
bool session::is_multiline_reply(const string & line) const
{
    if (line.size() < 4)
    {
        return false;
    }

    return line[3] == '-';
}

string session::read_control_connection()
{
    string line = read_line_control_connection();

    if (!is_multiline_reply(line))
    {
        return line;
    }

    string reply = line;
    string reply_code = get_reply_code(line);

    while (true)
    {
        line = read_line_control_connection();
        reply += line;

        /**
         * RFC 959: https://tools.ietf.org/html/rfc959
         *
         * The last line will begin with the same code, followed
         * immediately by Space <SP>, optionally some text, and the Telnet
         * end-of-line code.
         */
        string current_reply_code = get_reply_code(line);
        if (current_reply_code == reply_code && line.size() > 3 && line[3] == ' ')
        {
            break;
        }
    }

    return reply;
}

string session::read_line_control_connection()
{
    asio::read_until(control_connection_, read_buf_, '\n');
    istream is(&read_buf_);

    string line;
    getline(is, line);

    return line + "\n";
}

void session::write_control_connection(const string & command)
{
    asio::write(control_connection_, asio::buffer(command + "\r\n"));
}

bool session::control_connection_is_open() const
{
    return control_connection_.is_open();
}

} // namespace ftp
