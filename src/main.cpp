/**
 * main.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include <iostream>
#include "user_interface.hpp"
#include "resources.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::istream;
using std::exception;

int main(int argc, char * argv[])
{
    cin.exceptions(cin.exceptions() | istream::failbit | istream::badbit);

    try
    {
        ftp::user_interface ui;
        ui.run();
    }
    catch (const istream::failure & ex)
    {
        if (cin.eof())
        {
            return EXIT_SUCCESS;
        }
        else
        {
            cout << ex.what() << endl;
            return EXIT_FAILURE;
        }
    }
    catch (const exception & ex)
    {
        cout << ex.what() << endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        cout << "Unknown error." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
