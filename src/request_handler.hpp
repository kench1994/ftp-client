/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#ifndef FTP_CLIENT_REQUEST_HANDLER_HPP
#define FTP_CLIENT_REQUEST_HANDLER_HPP

#include <string>

using std::string;

namespace ftp
{

class request_handler
{
public:
    bool execute(const string & request);
};

} // namespace ftp
#endif //FTP_CLIENT_REQUEST_HANDLER_HPP
