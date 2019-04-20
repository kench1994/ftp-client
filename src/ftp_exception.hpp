/**
 * ftp_exception.hpp
 *
 * Copyright (c) 2019, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_FTP_EXCEPTION_HPP
#define FTP_CLIENT_FTP_EXCEPTION_HPP

#include "base_exception.hpp"

namespace ftp
{

class ftp_exception : public base_exception
{
public:
    template<typename ...Args>
    explicit ftp_exception(const std::string & message, Args && ...args)
        : base_exception(message, std::forward<Args>(args)...)
    {
    }
};

} // namespace ftp
#endif //FTP_CLIENT_FTP_EXCEPTION_HPP
