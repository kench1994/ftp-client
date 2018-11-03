#ifndef FTP_CLIENT_USER_INTERFACE_HPP
#define FTP_CLIENT_USER_INTERFACE_HPP

#include "request_handler.hpp"

class user_interface
{
public:
    void run();

private:
    request_handler request_handler_;
};

#endif //FTP_CLIENT_USER_INTERFACE_HPP
