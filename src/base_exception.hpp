/**
 * base_exception.hpp
 *
 * Copyright (c) 2019, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_BASE_EXCEPTION_HPP
#define FTP_CLIENT_BASE_EXCEPTION_HPP

#include <stdexcept>
#include <boost/format.hpp>

namespace ftp
{

class base_exception : public std::runtime_error
{
public:
    template<typename ...Args>
    explicit base_exception(const std::string & message, Args && ...args)
        : std::runtime_error(boost::str((boost::format(message) % ... % args)))
    {
    }
};

} // namespace ftp
#endif //FTP_CLIENT_BASE_EXCEPTION_HPP
