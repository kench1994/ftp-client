/**
 * resources.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "resources.hpp"

namespace ftp
{

using std::string;

namespace command
{
namespace local
{
    const string open = "open";
    const string user = "user";
    const string ls = "ls";
    const string close = "close";
    const string help = "help";
    const string exit = "exit";
} // namespace local

namespace remote
{
    const string user = "USER";
    const string password = "PASS";
    const string ls = "LIST";
    const string pasv = "PASV";
    const string close = "QUIT";
} // namespace remote
} // namespace command
} // namespace ftp
