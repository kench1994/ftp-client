/**
 * negative_code.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_NEGATIVE_COMPLETION_CODE_HPP
#define FTP_CLIENT_NEGATIVE_COMPLETION_CODE_HPP

#include "base_exception.hpp"

namespace ftp
{

class negative_completion_code : public base_exception
{
public:
    template<typename ...Args>
    explicit negative_completion_code(const std::string & message, Args && ...args)
        : base_exception(message, std::forward<Args>(args)...)
    {
    }
};

} // namespace ftp
#endif //FTP_CLIENT_NEGATIVE_COMPLETION_CODE_HPP
