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
    extern const std::string user;
    extern const std::string cd;
    extern const std::string ls;
    extern const std::string get;
    extern const std::string pwd;
    extern const std::string mkdir;
    extern const std::string syst;
    extern const std::string binary;
    extern const std::string size;
    extern const std::string close;
    extern const std::string help;
    extern const std::string exit;
} // namespace local

namespace remote
{
    extern const std::string user;
    extern const std::string password;
    extern const std::string cd;
    extern const std::string ls;
    extern const std::string get;
    extern const std::string pwd;
    extern const std::string mkdir;
    extern const std::string pasv;
    extern const std::string syst;
    extern const std::string binary;
    extern const std::string size;
    extern const std::string close;
} // namespace remote
} // namespace command
} // namespace ftp
#endif //FTP_CLIENT_RESOURCES_HPP
