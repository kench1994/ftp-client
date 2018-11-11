/**
 * request_handler.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "request_handler.hpp"
#include "resources.hpp"
#include <iostream>

using std::cout;
using std::endl;

namespace ftp
{

void request_handler::execute(const vector<string> & request)
{
    if (request.empty())
    {
        return;
    }

    const string & user_request = request[0];
    if (user_request == user_request::open)
    {
        open(request);
    }
    else if (user_request == user_request::close)
    {
        close();
    }
    else if (user_request == user_request::help)
    {
        help();
    }
    else if (user_request == user_request::exit)
    {
        exit();
    }
    else
    {
        throw std::runtime_error(error::invalid_request);
    }
}

void request_handler::open(const vector<string> & request)
{
    if (request.size() != 3)
    {
        throw std::runtime_error(usage::open);
    }

    const string & hostname = request[1];
    const string & port = request[2];
    session_.open_control_connection(hostname, port);
    cout << session_.read_control_connection();
}

void request_handler::close()
{
    if (!session_.control_connection_is_open())
    {
        throw std::runtime_error(error::not_connected);
    }

    session_.write_control_connection(ftp_request::close);
    cout << session_.read_control_connection();
    session_.close_control_connection();
}

void request_handler::help()
{
    cout << common::help;
}

void request_handler::exit()
{
    if (session_.control_connection_is_open())
    {
        close();
    }
}

} // namespace ftp
