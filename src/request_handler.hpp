#ifndef FTP_CLIENT_REQUEST_HANDLER_HPP
#define FTP_CLIENT_REQUEST_HANDLER_HPP

#include <string>

using std::string;

class request_handler
{
public:
    bool execute(const string & request);
};

#endif //FTP_CLIENT_REQUEST_HANDLER_HPP
