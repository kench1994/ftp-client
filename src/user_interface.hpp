/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#ifndef FTP_CLIENT_USER_INTERFACE_HPP
#define FTP_CLIENT_USER_INTERFACE_HPP

#include "request_handler.hpp"

namespace ftp
{

class user_interface
{
public:
    void run();

private:
    request_handler request_handler_;
};

} // namespace ftp
#endif //FTP_CLIENT_USER_INTERFACE_HPP
