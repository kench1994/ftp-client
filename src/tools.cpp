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

#include <iostream>
#include "tools.hpp"
#include <boost/filesystem.hpp>
#include <vector>
#include "ncurses_wrappers/ncurses_initializer.hpp"
#include "ncurses_wrappers/ncurses.hpp"

namespace tools
{

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using ncurses::ncurses_initializer;

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

string read_hidden_line(const string & greeting, int len)
{
    ncurses_initializer ncurses;
    ncurses::cbreak();

    ncurses::printw(greeting);
    ncurses::refresh();

    ncurses::noecho();

    vector<char> line(len + 1);
    ncurses::wgetnstr(stdscr, line.data(), len);

    ncurses::echo();
    ncurses::clear();

    return string(line.data());
}

string get_filename(const string & path)
{
    boost::filesystem::path p(path);
    return p.filename().string();
}

} // namespace tools
