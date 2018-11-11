/**
 * command_handler.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_COMMAND_HANDLER_HPP
#define FTP_CLIENT_COMMAND_HANDLER_HPP

#include <string>
#include <vector>
#include "session.hpp"

using std::string;
using std::vector;

namespace ftp
{

class command_handler
{
public:
    void execute(const vector<string> & command);

private:
    bool is_local_command(const string & command) const;

    bool is_remote_command(const string & command) const;

    void execute_local_command(const vector<string> & command);

    void execute_remote_command(const vector<string> & command);

    void open(const vector<string> & command);

    void close();

    void help();

    void exit();

    session session_;
};

} // namespace ftp
#endif //FTP_CLIENT_command_HANDLER_HPP
