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

#ifndef FTP_CLIENT_COMMANDS_HPP
#define FTP_CLIENT_COMMANDS_HPP

#include <string>

namespace command
{
    inline const char * open = "open";
    inline const char * user = "user";
    inline const char * cd = "cd";
    inline const char * ls = "ls";
    inline const char * get = "get";
    inline const char * pwd = "pwd";
    inline const char * mkdir = "mkdir";
    inline const char * syst = "syst";
    inline const char * binary = "binary";
    inline const char * size = "size";
    inline const char * noop = "noop";
    inline const char * close = "close";
    inline const char * help = "help";
    inline const char * exit = "exit";
} // namespace command
#endif //FTP_CLIENT_COMMANDS_HPP
