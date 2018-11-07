/**
 * user_interface.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "user_interface.hpp"
#include "request_parser.hpp"
#include "resources.hpp"
#include <iostream>

using std::cout;
using std::cin;
using std::endl;
using std::string;

namespace ftp
{

void user_interface::run()
{
    string request;

    while (true)
    {
        cout << "ftp> ";
        getline(cin, request);

        vector<string> parsed_request = request_parser::parse(request);
        if (parsed_request.empty())
        {
            continue;
        }

        try
        {
            request_handler_.execute(parsed_request);
        }
        catch (const std::exception & ex)
        {
            cout << ex.what() << endl;
        }

        const string & user_request = parsed_request[0];
        if (user_request == user_request::exit)
        {
            break;
        }
    }
}

} // namespace ftp
