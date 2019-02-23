/**
 * command_handler.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_COMMAND_HANDLER_HPP
#define FTP_CLIENT_COMMAND_HANDLER_HPP

#include <string>
#include <vector>
#include "client.hpp"

namespace ftp
{

class command_handler
{
public:
    void execute(const std::string & command,
                 const std::vector<std::string> & arguments);

private:
    bool is_needed_connection(const std::string & command) const;

    void open(const std::vector<std::string> & arguments);

    void user();

    void user(const std::vector<std::string> & arguments);

    void pass();

    void cd(const std::vector<std::string> & arguments);

    void ls(const std::vector<std::string> & arguments);

    void pwd();

    void mkdir(const std::vector<std::string> & arguments);

    void syst();

    void close();

    void help();

    void exit();

    client client_;
};

} // namespace ftp
#endif //FTP_CLIENT_COMMAND_HANDLER_HPP
