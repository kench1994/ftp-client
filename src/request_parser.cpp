/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#include "request_parser.hpp"
#include <sstream>

using std::istringstream;

namespace ftp
{

vector<string> request_parser::parse(const string & request)
{
    vector<string> parsed_request;
    istringstream iss(request);

    for (string parameter; iss >> parameter;)
    {
        parsed_request.push_back(parameter);
    }

    return parsed_request;
}

} // namespace ftp
