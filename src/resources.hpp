/**
 * resources.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_RESOURCES_HPP
#define FTP_CLIENT_RESOURCES_HPP

#include <string>

namespace ftp
{
namespace command
{
namespace local
{
    extern const std::string open;
    extern const std::string close;
    extern const std::string help;
    extern const std::string exit;
} // namespace local

namespace remote
{
    extern const std::string user;
    extern const std::string password;
    extern const std::string close;
} // namespace remote
} // namespace command

namespace error
{
    extern const std::string unknown_error;
    extern const std::string invalid_command;
    extern const std::string not_connected;
    extern const std::string already_connected;
} // namespace error

namespace usage
{
    extern const std::string open;
} // namespace usage

namespace common
{
    extern const std::string ftp_prefix;
    extern const std::string enter_name;
    extern const std::string enter_password;
    extern const std::string help;
} // namespace common
} // namespace ftp
#endif //FTP_CLIENT_RESOURCES_HPP
