/**
 * utils.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include <iostream>
#include "utils.hpp"

using std::cout;
using std::cin;

namespace ftp
{
namespace utils
{

string read_line(const string & greeting)
{
    string line;

    while (line.empty())
    {
        cout << greeting;
        getline(cin, line);
    }

    return line;
}

} // namespace utils
} // namespace ftp
