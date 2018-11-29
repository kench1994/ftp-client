/**
 * command.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_USER_COMMAND_HPP
#define FTP_CLIENT_USER_COMMAND_HPP

#include <string>
#include <vector>

namespace ftp
{

class user_command
{
public:
    user_command() = default;

    user_command(std::string && command, std::vector<std::string> && parameters)
            : command_(std::move(command)),
              parameters_(std::move(parameters))
    {
    }

    const std::string & command() const
    {
        return command_;
    }

    const std::vector<std::string> & parameters() const
    {
        return parameters_;
    }

    friend bool operator==(const user_command & lhs, const std::string & rhs)
    {
        return lhs.command() == rhs;
    }

private:
    std::string command_;
    std::vector<std::string> parameters_;
};

} // namespace ftp
#endif //FTP_CLIENT_USER_COMMAND_HPP
