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

#include "ncurses.hpp"
#include "ncurses_exception.hpp"

namespace ncurses
{

using std::string;

void cbreak()
{
    if (::cbreak() ==  ERR)
        throw ncurses_exception("ncurses: cannot activate cbreak mode.");
}

void printw(const string & str)
{
    if (::printw(str.data()) == ERR)
        throw ncurses_exception("ncurses: cannot print.");
}

void echo()
{
    if (::echo() == ERR)
        throw ncurses_exception("ncurses: cannot activate echo mode.");
}

void noecho()
{
    if (::noecho() == ERR)
        throw ncurses_exception("ncurses: cannot activate noecho mode.");
}

void clear()
{
    if (::clear() == ERR)
        throw ncurses_exception("ncurses: cannot clear.");
}

void wgetnstr(WINDOW *window, char *str, int len)
{
    if (::wgetnstr(window, str, len) == ERR)
        throw ncurses_exception("ncurses: cannot get string.");
}

void refresh()
{
    if (::refresh() == ERR)
        throw ncurses_exception("ncurses: cannot refresh.");
}

} // namespace ncurses
