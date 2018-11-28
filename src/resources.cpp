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
namespace command
{
    const std::string open = "open";
    const std::string close = "close";
    const std::string help = "help";
    const std::string exit = "exit";
} // namespace command

namespace ftp_command
{
    const std::string user = "USER";
    const std::string password = "PASS";
    const std::string close = "QUIT";
} // namespace ftp_command

namespace error
{
    const std::string unknown_error = "Unknown error.";
    const std::string invalid_command =
        "Invalid command. Use 'help' to display list of FTP commands.";
    const std::string not_connected = "Not connected.";
    const std::string already_connected = "Already connected, use close first.";
} // namespace error

namespace usage
{
    const std::string open = "Usage: open <hostname> <port>";
} // namespace usage

namespace common
{
    const std::string ftp_prefix = "ftp> ";
    const std::string enter_name = "login: ";
    const std::string enter_password = "password: ";
    const std::string help =
        "List of FTP commands:\n"
        "\topen <hostname> <open> - Open new connection.\n"
        "\tclose - Close current connection.\n"
        "\thelp - Print list of FTP commands.\n"
        "\texit - Exit program.\n";
} // namespace common
} // namespace ftp
