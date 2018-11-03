#include "request_parser.hpp"
#include <sstream>

using std::istringstream;

vector<string> request_parser::parse(const string & request)
{
    vector <string> parsed_request;
    istringstream iss(request);

    for (string parameter; iss >> parameter;)
    {
        parsed_request.push_back(parameter);
    }

    return parsed_request;
}
