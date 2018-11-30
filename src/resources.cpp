/**
 * resources.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "resources.hpp"

namespace ftp
{

using std::string;

namespace command
{
    const string open = "open";
    const string close = "close";
    const string help = "help";
    const string exit = "exit";
} // namespace command

namespace ftp_command
{
    const string user = "USER";
    const string password = "PASS";
    const string close = "QUIT";
} // namespace ftp_command

namespace error
{
    const string unknown_error = "Unknown error.";
    const string invalid_command =
        "Invalid command. Use 'help' to display list of FTP commands.";
    const string not_connected = "Not connected.";
    const string already_connected = "Already connected, use close first.";
} // namespace error

namespace usage
{
    const string open = "Usage: open <hostname> <port>";
} // namespace usage

namespace common
{
    const string ftp_prefix = "ftp> ";
    const string enter_name = "login: ";
    const string enter_password = "password: ";
    const string help =
        "List of FTP commands:\n"
        "\topen <hostname> <open> - Open new connection.\n"
        "\tclose - Close current connection.\n"
        "\thelp - Print list of FTP commands.\n"
        "\texit - Exit program.\n";
} // namespace common
} // namespace ftp
