/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#include "request_handler.hpp"
#include "request_parser.hpp"
#include "requests.hpp"

namespace ftp
{

bool request_handler::execute(const string & request)
{
    if (request.empty())
    {
        return true;
    }

    vector<string> parsed_request = request_parser::parse(request);
    const string & ftp_request = parsed_request[0];

    if (ftp_request == request::open)
    {
        open(parsed_request);
    }

    return parsed_request[0] != request::exit;
}

void request_handler::open(const vector<string> & request)
{
    const string & hostname = request[1];
    const string & port = request[2];
    session_.open_control_connection(hostname, port);
}

} // namespace ftp
