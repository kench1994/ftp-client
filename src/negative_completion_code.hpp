/**
 * negative_code.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_NEGATIVE_COMPLETION_CODE_HPP
#define FTP_CLIENT_NEGATIVE_COMPLETION_CODE_HPP

#include <stdexcept>

namespace ftp
{

class negative_completion_code : public std::runtime_error
{
public:
    explicit negative_completion_code(const std::string & message)
            : std::runtime_error(message)
    {
    }
};

} // namespace ftp
#endif //FTP_CLIENT_NEGATIVE_COMPLETION_CODE_HPP
