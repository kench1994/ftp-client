/**
 * local_exception.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_LOCAL_EXCEPTION_HPP
#define FTP_CLIENT_LOCAL_EXCEPTION_HPP

#include "base_exception.hpp"

namespace ftp
{

class local_exception : public base_exception
{
public:
    template<typename ...Args>
    explicit local_exception(const std::string & message, Args && ...args)
        : base_exception(message, std::forward<Args>(args)...)
    {
    }
};

} // namespace ftp
#endif //FTP_CLIENT_LOCAL_EXCEPTION_HPP
