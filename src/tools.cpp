/**
 * tools.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include <iostream>
#include <zconf.h>
#include <termio.h>
#include "tools.hpp"
#include <boost/filesystem.hpp>

namespace ftp
{
namespace tools
{

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;

string read_line(const string & greeting)
{
    string line;

    cout << greeting;
    getline(cin, line);

    return line;
}

string read_not_empty_line(const string & greeting)
{
    string line;

    while (line.empty())
    {
        cout << greeting;
        getline(cin, line);
    }

    return line;
}

string read_secure_line(const string & greeting)
{
    struct termios old_settings;
    struct termios new_settings;

    /**
     * Masking password input.
     * http://www.cplusplus.com/articles/E6vU7k9E
     * http://man7.org/linux/man-pages/man3/termios.3.html
     */
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    string password;
    cout << greeting;
    getline(cin, password);

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

    cout << endl;

    return password;
}

string get_filename(const string & path)
{
    boost::filesystem::path p(path);
    return p.filename().string();
}

} // namespace tools
} // namespace ftp
