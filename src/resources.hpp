/**
 * resources.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_RESOURCES_HPP
#define FTP_CLIENT_RESOURCES_HPP

namespace ftp
{
namespace command
{
    extern const char * open;
    extern const char * close;
    extern const char * help;
    extern const char * exit;
} // namespace command

namespace ftp_command
{
    extern const char * user;
    extern const char * password;
    extern const char * close;
} // namespace ftp_command

namespace error
{
    extern const char * invalid_command;
    extern const char * not_connected;
    extern const char * already_connected;
} // namespace error

namespace usage
{
    extern const char * open;
} // namespace usage

namespace common
{
    extern const char * ftp_prefix;
    extern const char * enter_name;
    extern const char * enter_password;
    extern const char * help;
} // namespace common
} // namespace ftp
#endif //FTP_CLIENT_RESOURCES_HPP
