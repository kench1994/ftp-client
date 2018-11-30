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

namespace ftp
{

control_connection::control_connection(const std::string & hostname,
                                       const std::string & port)
        : socket_(io_context_),
          resolver_(io_context_)
{
    asio::connect(socket_, resolver_.resolve(hostname, port));
}

control_connection::~control_connection()
{
    socket_.shutdown(asio::socket_base::shutdown_both);
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * An FTP reply consists of a three digit number (transmitted as
 * three alphanumeric characters) followed by some text.
 */
std::string control_connection::get_reply_code(const std::string & line)
{
    if (line.size() < 3)
    {
        return std::string();
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
bool control_connection::is_multiline_reply(const std::string & line) const
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
bool control_connection::is_end_of_multiline_reply(const std::string & line,
                                                   const std::string & first_reply_code)
{
    std::string current_reply_code = get_reply_code(line);
    return current_reply_code == first_reply_code && line.size() > 3 && line[3] == ' ';
}

std::string control_connection::read()
{
    std::string line = read_line();

    if (!is_multiline_reply(line))
    {
        return line;
    }

    std::string reply = line;
    std::string first_reply_code = get_reply_code(line);

    while (!is_end_of_multiline_reply(line, first_reply_code))
    {
        line = read_line();
        reply += line;
    }

    return reply;
}

std::string control_connection::read_line()
{
    asio::read_until(socket_, read_buf_, '\n');
    std::istream is(&read_buf_);

    std::string line;
    getline(is, line);

    return line + "\n";
}

void control_connection::write(const std::string & command)
{
    asio::write(socket_, asio::buffer(command + "\r\n"));
}

} // namespace ftp
