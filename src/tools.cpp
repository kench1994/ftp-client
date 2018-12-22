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
#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "tools.hpp"

namespace ftp
{
namespace tools
{

using std::string;
using std::vector;
using std::istringstream;
using std::cout;
using std::cin;
using std::endl;

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

string get_command(const string & user_input)
{
    string command;

    istringstream iss(user_input);
    iss >> command;

    return command;
}

vector<string> get_arguments(const string & user_input)
{
    vector<string> arguments;
    boost::split(arguments, user_input, boost::is_any_of(" "));

    if (!arguments.empty())
    {
        arguments.erase(arguments.begin());
    }

    return arguments;
}

void add_line(string & source, const string & line)
{
    if (!source.empty())
    {
        source += "\n";
    }

    source += line;
}

} // namespace tools
} // namespace ftp
