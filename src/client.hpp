/**
 * client.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_CLIENT_HPP
#define FTP_CLIENT_CLIENT_HPP

#include <string>
#include <vector>
#include "control_connection.hpp"
#include "data_transfer_mode.hpp"
#include <optional>
#include <fstream>

namespace ftp
{

class client
{
public:
    void open(const std::string & hostname, const std::string & port = "21");

    bool is_open() const;

    void user(const std::string & username);

    void pass(const std::string & password);

    void cd(const std::string & remote_directory);

    void list(const std::optional<std::string> & remote_directory = std::nullopt);

    void get(const std::string & remote_path, std::ofstream & file);

    void pwd();

    void mkdir(const std::string & pathname);

    void binary();

    void size(const std::string & filename);

    void syst();

    void close();

    void reset();

private:
    boost::asio::io_context io_context_;
    std::unique_ptr<control_connection> control_connection_;
    std::unique_ptr<data_transfer_mode> data_transfer_mode_;
};

} // namespace ftp
#endif //FTP_CLIENT_CLIENT_HPP
