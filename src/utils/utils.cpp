/*
 * MIT License
 *
 * Copyright (c) 2019 Denis Kovalchuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "utils.hpp"
#include <iostream>
#include <termios.h>

namespace utils
{

using std::string;
using std::cout;
using std::cin;

string read_line(const string & greeting)
{
    string line;

    cout << greeting;
    getline(cin, line);

    return line;
}

string read_password(const string & greeting)
{
    struct termios old_settings = {0};
    struct termios new_settings = {0};

    tcgetattr(STDIN_FILENO, &old_settings);

    new_settings = old_settings;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag |= ECHONL;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    string line;
    cout << greeting;
    getline(cin, line);

    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

    return line;
}

string get_filename(const string & path)
{
    size_t last_slash = path.find_last_of("\\/");

    if (last_slash == string::npos)
    {
        return path;
    }
    else
    {
        return path.substr(last_slash + 1);
    }
}

} // namespace utils
