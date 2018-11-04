/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
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
    bool execute(const string & request);

private:
    void open(const vector<string> & request);

    session session_;
};

} // namespace ftp
#endif //FTP_CLIENT_REQUEST_HANDLER_HPP
