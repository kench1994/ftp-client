/**
 * command.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_COMMAND_HPP
#define FTP_CLIENT_COMMAND_HPP

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace ftp
{

class user_command
{
public:
    user_command() = default;

    user_command(string && command, vector<string> && parameters)
            : command_(std::move(command)),
              parameters_(std::move(parameters))
    {
    }

    const string & command() const
    {
        return command_;
    }

    const vector<string> & parameters() const
    {
        return parameters_;
    }

    friend bool operator==(const user_command & lhs, const char * rhs)
    {
        return lhs.command() == rhs;
    }

private:
    string command_;
    vector<string> parameters_;
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_HPP
