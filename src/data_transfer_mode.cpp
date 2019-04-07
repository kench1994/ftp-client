/**
 * data_transfer_mode.cpp
 *
 * Copyright (c) 2019, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include <memory>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "data_transfer_mode.hpp"
#include "resources.hpp"
#include "local_exception.hpp"

namespace ftp
{

using std::unique_ptr;
using std::make_unique;
using std::string;
using std::cout;
using std::endl;
using std::vector;

data_transfer_mode::data_transfer_mode(boost::asio::io_context & io_context)
    : io_context_(io_context)
{
}

passive_mode::passive_mode(boost::asio::io_context & io_context)
    : data_transfer_mode(io_context)
{
}

unique_ptr<data_connection> passive_mode::open_data_connection(control_connection & control_connection)
{
    control_connection.write(command::remote::pasv);
    string server_reply = control_connection.read();

    cout << server_reply << endl;

    boost::asio::ip::tcp::endpoint server_endpoint =
            get_endpoint_from_server_reply(server_reply);

    return make_unique<data_connection>(io_context_, server_endpoint);
}

/**
 * RFC 959: https://tools.ietf.org/html/rfc959
 *
 * This address information is broken into 8-bit fields and the
 * value of each field is transmitted as a decimal number (in
 * character string representation).  The fields are separated
 * by commas.
 *
 * Format of server reply: 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
 *
 * Where h1 is the high order 8 bits of the internet host address.
 */
boost::asio::ip::tcp::endpoint
passive_mode::get_endpoint_from_server_reply(const string & server_reply)
{
    size_t left_bracket = server_reply.find('(');
    if (left_bracket == string::npos)
    {
        throw local_exception("Invalid reply of the PASV command.");
    }

    size_t right_bracket = server_reply.find(')');
    if (right_bracket == string::npos)
    {
        throw local_exception("Invalid reply of the PASV command.");
    }

    // Transform from: 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
    //            to:  h1,h2,h3,h4,p1,p2
    string ip_port = server_reply.substr(left_bracket + 1, right_bracket - left_bracket - 1);

    vector<string> numbers;
    boost::split(numbers, ip_port, boost::is_any_of(","));

    static size_t needed_numbers_count = 6;
    if (numbers.size() != needed_numbers_count)
    {
        throw local_exception("Invalid reply of the PASV command.");
    }

    string ip = numbers[0] + '.' + numbers[1] + '.' + numbers[2] + '.' + numbers[3];
    uint16_t port = (boost::lexical_cast<uint16_t>(numbers[4]) * uint16_t (256)) +
                     boost::lexical_cast<uint16_t>(numbers[5]);

    return boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string(ip), port);
}

} // namespace ftp
