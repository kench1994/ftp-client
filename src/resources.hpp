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
namespace user_request
{
    extern const char * open;
    extern const char * close;
    extern const char * exit;
} // namespace user_request

namespace ftp_request
{
    extern const char * close;
} // namespace ftp_request

namespace error
{
    extern const char * invalid_request;
    extern const char * not_connected;
} // namespace error

namespace usage
{
    extern const char * open;
} // namespace usage
} // namespace ftp
#endif //FTP_CLIENT_RESOURCES_HPP
