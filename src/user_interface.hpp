/**
 * user_interface.hpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_USER_INTERFACE_HPP
#define FTP_CLIENT_USER_INTERFACE_HPP

#include "client.hpp"

namespace ftp
{

class user_interface
{
public:
    void run();

private:
    client ftp_client_;
};

} // namespace ftp
#endif //FTP_CLIENT_USER_INTERFACE_HPP
