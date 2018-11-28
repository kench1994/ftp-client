/**
 * main.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include <iostream>
#include "user_interface.hpp"

int main(int argc, char * argv[])
{
    std::cin.exceptions(std::cin.exceptions() | std::istream::failbit | std::istream::badbit);

    try
    {
        ftp::user_interface ui;
        ui.run();
    }
    catch (const std::istream::failure & ex)
    {
        if (std::cin.eof())
        {
            return EXIT_SUCCESS;
        }
        else
        {
            std::cout << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
    catch (const std::exception & ex)
    {
        std::cout << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
