/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#include "user_interface.hpp"
#include <iostream>

using std::cout;
using std::cin;
using std::string;

namespace ftp
{

void user_interface::run()
{
    string request;
    bool proceed = true;

    while (proceed)
    {
        cout << "ftp> ";
        getline(cin, request);
        proceed = request_handler_.execute(request);
    }
}

} // namespace ftp
