/**
 * main.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "user_interface.hpp"

int main(int argc, char * argv[])
{
    ftp::user_interface ui;
    ui.run();

    return 0;
}
