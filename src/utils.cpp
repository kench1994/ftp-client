/**
 * utils.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include <iostream>
#include <zconf.h>
#include <termio.h>
#include "utils.hpp"

namespace ftp
{
namespace utils
{

std::string read_line(const std::string & greeting)
{
    std::string line;

    while (line.empty())
    {
        std::cout << greeting;
        getline(std::cin, line);
    }

    return line;
}

std::string read_secure_line(const std::string & greeting)
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

    std::string password;
    std::cout << greeting;
    getline(std::cin, password);

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

    std::cout << std::endl;

    return password;
}

} // namespace utils
} // namespace ftp
