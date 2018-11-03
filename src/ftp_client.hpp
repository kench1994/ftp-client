/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#ifndef FTP_CLIENT_CLIENT_HPP
#define FTP_CLIENT_CLIENT_HPP

#include <string>

using std::string;

namespace ftp
{

class client
{
public:
    client(const string & hostname, unsigned short port);

private:
    string hostname_;
    unsigned short port_;
};

} // namespace ftp
#endif //FTP_CLIENT_CLIENT_HPP
