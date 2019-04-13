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
    const string cd = "cd";
    const string ls = "ls";
    const string get = "get";
    const string pwd = "pwd";
    const string mkdir = "mkdir";
    const string syst = "syst";
    const string binary = "binary";
    const string size = "size";
    const string close = "close";
    const string help = "help";
    const string exit = "exit";
} // namespace local

namespace remote
{
    const string user = "USER";
    const string password = "PASS";
    const string cd = "CWD";
    const string ls = "LIST";
    const string get = "RETR";
    const string pwd = "PWD";
    const string mkdir = "MKD";
    const string pasv = "PASV";
    const string syst = "SYST";
    const string binary = "TYPE I";
    const string size = "SIZE";
    const string close = "QUIT";
} // namespace remote
} // namespace command
} // namespace ftp
