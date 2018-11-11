/**
 * request_handler.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#ifndef FTP_CLIENT_REQUEST_HANDLER_HPP
#define FTP_CLIENT_REQUEST_HANDLER_HPP

#include <string>
#include <vector>
#include "session.hpp"

using std::string;
using std::vector;

namespace ftp
{

class request_handler
{
public:
    void execute(const vector<string> & request);

private:
    void open(const vector<string> & request);

    void close();

    void help();

    void exit();

    session session_;
};

} // namespace ftp
#endif //FTP_CLIENT_REQUEST_HANDLER_HPP
