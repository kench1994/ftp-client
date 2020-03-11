/*
 * MIT License
 *
 * Copyright (c) 2019 Denis Kovalchuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef FTP_CONNECTION_EXCEPTION_HPP
#define FTP_CONNECTION_EXCEPTION_HPP

#include "utils.hpp"

namespace ftp::detail
{

class connection_exception : public std::exception
{
public:
    explicit connection_exception(boost::system::error_code & ec)
    {
        message_ = ec.message();
    }

    template<typename ...Args>
    explicit connection_exception(boost::system::error_code & ec, const std::string & fmt, Args && ...args)
    {
        message_ = detail::utils::format(fmt, std::forward<Args>(args)...);
        message_.append(": ");
        message_.append(ec.message());
    }

    template<typename ...Args>
    explicit connection_exception(const std::string & fmt, Args && ...args)
    {
        message_ = detail::utils::format(fmt, std::forward<Args>(args)...);
    }

    const char * what() const noexcept override
    {
        return message_.c_str();
    }

private:
    std::string message_;
};

} // namespace ftp::detail
#endif //FTP_CONNECTION_EXCEPTION_HPP
