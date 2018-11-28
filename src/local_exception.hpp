/**
 * local_exception.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_LOCAL_EXCEPTION_HPP
#define FTP_CLIENT_LOCAL_EXCEPTION_HPP

#include <stdexcept>

namespace ftp
{

class local_exception : public std::runtime_error
{
public:
    explicit local_exception(const std::string & message)
            : std::runtime_error(message)
    {
    }
};

} // namespace ftp
#endif //FTP_CLIENT_LOCAL_EXCEPTION_HPP
