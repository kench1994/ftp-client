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

#include <boost/lexical_cast.hpp>
#include "client.hpp"
#include "ftp_exception.hpp"

namespace ftp
{

using std::string;
using std::vector;
using std::make_unique;
using std::optional;
using std::ofstream;
using std::unique_ptr;

void client::open(const string & hostname, uint16_t port)
{
    control_connection_.open(hostname, port);
    notify_observers(control_connection_.recv());
}

bool client::is_open() const
{
    return control_connection_.is_open();
}

void client::user(const string & username, const string & password)
{
    control_connection_.send("USER " + username);
    notify_observers(control_connection_.recv());

    /**
     * Send PASS command.
     *
     * This command must be immediately preceded by the
     * user name command, and, for some sites, completes the user's
     * identification for access control.
     *
     * RFC 959: https://tools.ietf.org/html/rfc959
     */
    control_connection_.send("PASS " + password);
    notify_observers(control_connection_.recv());
}

void client::cd(const string & remote_directory)
{
    control_connection_.send("CWD " + remote_directory);
    notify_observers(control_connection_.recv());
}

void client::ls(const string & remote_directory)
{
    string command;

    if (remote_directory.empty())
    {
        command = "LIST";
    }
    else
    {
        command = "LIST " + remote_directory;
    }

    unique_ptr<data_connection> data_connection = create_data_connection();

    control_connection_.send(command);
    notify_observers(control_connection_.recv());

    notify_observers(data_connection->recv());
    // Don't keep the data connection.
    data_connection.reset();

    notify_observers(control_connection_.recv());
}

void client::get(const string & remote_file, ofstream & file)
{
    unique_ptr<data_connection> data_connection = create_data_connection();

    control_connection_.send("RETR " + remote_file);
    notify_observers(control_connection_.recv());

    data_connection->recv_file(file);
    // Don't keep the data connection.
    data_connection.reset();

    notify_observers(control_connection_.recv());
}

void client::pwd()
{
    control_connection_.send("PWD");
    notify_observers(control_connection_.recv());
}

void client::mkdir(const string & directory_name)
{
    control_connection_.send("MKD " + directory_name);
    notify_observers(control_connection_.recv());
}

void client::binary()
{
    control_connection_.send("TYPE I");
    notify_observers(control_connection_.recv());
}

void client::size(const string & remote_file)
{
    control_connection_.send("SIZE " + remote_file);
    notify_observers(control_connection_.recv());
}

void client::syst()
{
    control_connection_.send("SYST");
    notify_observers(control_connection_.recv());
}

void client::noop()
{
    control_connection_.send("NOOP");
    notify_observers(control_connection_.recv());
}

void client::close()
{
    control_connection_.send("QUIT");
    notify_observers(control_connection_.recv());
    control_connection_.close();
}

unique_ptr<data_connection> client::create_data_connection()
{
    control_connection_.send("EPSV");
    string reply = control_connection_.recv();

    notify_observers(reply);

    uint16_t port = parse_epsv_port(reply);

    unique_ptr<data_connection> connection = make_unique<data_connection>();

    connection->open(control_connection_.ip(), port);

    return connection;
}

/**
 * The text returned in response to the EPSV command MUST be:
 *
 *     <text indicating server is entering extended passive mode> \
 *       (<d><d><d><tcp-port><d>)
 *
 * The first two fields contained in the parenthesis MUST be blank.  The
 * third field MUST be the string representation of the TCP port number
 * on which the server is listening for a data connection.  The network
 * protocol used by the data connection will be the same network
 * protocol used by the control connection.  In addition, the network
 * address used to establish the data connection will be the same
 * network address used for the control connection.  An example response
 * string follows:
 *
 *     Entering Extended Passive Mode (|||6446|)
 *
 * RFC 2428: https://tools.ietf.org/html/rfc2428
 */
uint16_t client::parse_epsv_port(const string & reply)
{
    uint16_t port;
    size_t begin;
    size_t end;

    begin = reply.find('|');
    if (begin == string::npos)
    {
        throw ftp_exception("Invalid server reply: %1%", reply);
    }

    // Skip '|||' characters.
    begin = begin + 3;

    end = reply.rfind('|');
    if (end == string::npos)
    {
        throw ftp_exception("Invalid server reply: %1%", reply);
    }

    string port_str = reply.substr(begin, end - begin);
    if (!boost::conversion::try_lexical_convert(port_str, port))
    {
        throw ftp_exception("Invalid server reply: %1%", reply);
    }

    return port;
}

void client::add_observer(reply_observer *observer)
{
    observers_.push_back(observer);
}

void client::remove_observer(reply_observer *observer)
{
    observers_.remove(observer);
}

void client::notify_observers(const string & reply)
{
    for (const auto & observer : observers_)
        observer->handle_reply(reply);
}

} // namespace ftp
