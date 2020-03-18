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

#include "cmdline_interface.hpp"
#include <iostream>

using std::cerr;
using std::cin;
using std::endl;
using std::istream;
using std::exception;

int main(int argc, char *argv[])
{
    cin.exceptions(cin.exceptions() | istream::failbit | istream::badbit);

    try
    {
        cmdline_interface cmdline;
        cmdline.run();
    }
    catch (const istream::failure & ex)
    {
        if (cin.eof())
        {
            return EXIT_SUCCESS;
        }
        else
        {
            cerr << ex.what() << endl;
            return EXIT_FAILURE;
        }
    }
    catch (const exception & ex)
    {
        cerr << ex.what() << endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        cerr << "Unknown error." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
