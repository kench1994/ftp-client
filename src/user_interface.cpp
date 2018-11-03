#include "user_interface.hpp"

#include <iostream>
#include <string>

using std::cout;
using std::cin;
using std::string;

void user_interface::run()
{
    string request;

    do
    {
        cout << "ftp> ";
        getline(cin, request);
    } while (request_handler_.execute(request));
}
