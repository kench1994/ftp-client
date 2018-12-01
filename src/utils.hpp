/**
 * utils.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_UTILS_HPP
#define FTP_CLIENT_UTILS_HPP

#include <string>
#include <vector>

namespace ftp
{
namespace utils
{

std::string read_line(const std::string & greeting);

std::string read_secure_line(const std::string & greeting);

std::string get_command(const std::string & user_input);

std::vector<std::string> get_arguments(const std::string & user_input);

} // namespace utils
} // namespace ftp
#endif //FTP_CLIENT_UTILS_HPP
