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

#include "client.hpp"
#include "detail/utils.hpp"
#include <boost/lexical_cast.hpp>

namespace ftp
{
using std::string;
using std::vector;
using std::make_unique;
using std::optional;
using std::ifstream;
using std::ofstream;
using std::unique_ptr;
using std::ios_base;

using namespace ftp::detail;

void client::open(const string & hostname, uint16_t port)
{
    try
    {
        reply_t reply;

        control_connection_.open(hostname, port);
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

bool client::is_open() const
{
    return control_connection_.is_open();
}

void client::user(const string & username, const string & password)
{
    try
    {
        reply_t reply;

        control_connection_.send("USER " + username);
        reply = control_connection_.recv();

        notify_of_reply(reply);

        if (reply.status_code == 331)
        {
            // 331 User name okay, need password.
            control_connection_.send("PASS " + password);
            notify_of_reply(control_connection_.recv());
        }
        else if (reply.status_code == 332)
        {
            /* 332 Need account for login.
             * Sorry, we don't support ACCT command.
             */
        }
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::cwd(const string & remote_directory)
{
    try
    {
        reply_t reply;

        control_connection_.send("CWD " + remote_directory);
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::list(const optional<string> & remote_directory)
{
    string command;

    if (remote_directory)
    {
        command = "LIST " + remote_directory.value();
    }
    else
    {
        command = "LIST";
    }

    try
    {
        unique_ptr<data_connection> data_connection = create_data_connection();

        if (!data_connection)
        {
            return;
        }

        reply_t reply;

        control_connection_.send(command);
        reply = control_connection_.recv();

        notify_of_reply(reply);

        if (reply.status_code >= 400)
        {
            return;
        }

        notify_of_reply(data_connection->recv());
        // Don't keep the data connection.
        data_connection->close();

        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::stor(const string & local_file, const string & remote_file)
{
    ifstream file(local_file, ios_base::binary);

    if (!file)
    {
        string error_message = utils::format("Cannot open file: '%1%'.", local_file);
        notify_of_error(error_message);
        return;
    }

    try
    {
        unique_ptr<data_connection> data_connection = create_data_connection();

        if (!data_connection)
        {
            return;
        }

        reply_t reply;

        control_connection_.send("STOR " + remote_file);
        reply = control_connection_.recv();

        notify_of_reply(reply);

        if (reply.status_code >= 400)
        {
            return;
        }

        data_connection->send_file(file);
        // Don't keep the data connection.
        data_connection->close();

        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::retr(const string & remote_file, const string & local_file)
{
    ofstream file(local_file, ios_base::binary);

    if (!file)
    {
        string error_message = utils::format("Cannot create file: '%1%'.", local_file);
        notify_of_error(error_message);
        return;
    }

    try
    {
        unique_ptr<data_connection> data_connection = create_data_connection();

        if (!data_connection)
        {
            return;
        }

        reply_t reply;

        control_connection_.send("RETR " + remote_file);
        reply = control_connection_.recv();

        notify_of_reply(reply);

        if (reply.status_code >= 400)
        {
            return;
        }

        data_connection->recv_file(file);
        // Don't keep the data connection.
        data_connection->close();

        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::pwd()
{
    try
    {
        reply_t reply;

        control_connection_.send("PWD");
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::mkd(const string & directory_name)
{
    try
    {
        reply_t reply;

        control_connection_.send("MKD " + directory_name);
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::rmd(const string & directory_name)
{
    try
    {
        reply_t reply;

        control_connection_.send("RMD " + directory_name);
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::dele(const string & remote_file)
{
    try
    {
        reply_t reply;

        control_connection_.send("DELE " + remote_file);
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::type_i()
{
    try
    {
        reply_t reply;

        control_connection_.send("TYPE I");
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::size(const string & remote_file)
{
    try
    {
        reply_t reply;

        control_connection_.send("SIZE " + remote_file);
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::stat(const optional<string> & remote_file)
{
    string command;

    if (remote_file)
    {
        command = "STAT " + remote_file.value();
    }
    else
    {
        command = "STAT";
    }

    try
    {
        reply_t reply;

        control_connection_.send(command);
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::syst()
{
    try
    {
        reply_t reply;

        control_connection_.send("SYST");
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::noop()
{
    try
    {
        reply_t reply;

        control_connection_.send("NOOP");
        reply = control_connection_.recv();

        notify_of_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::quit()
{
    try
    {
        reply_t reply;

        control_connection_.send("QUIT");
        reply = control_connection_.recv();

        notify_of_reply(reply);

        control_connection_.close();
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

unique_ptr<data_connection> client::create_data_connection()
{
    try
    {
        reply_t reply;

        control_connection_.send("EPSV");
        reply = control_connection_.recv();

        notify_of_reply(reply);

        if (reply.status_code >= 400)
        {
            return nullptr;
        }

        uint16_t port;
        if (!try_parse_server_port(reply.status_line, port))
        {
            string error_msg = utils::format("Cannot parse server port from '%1%'.",
                                             reply.status_line);
            notify_of_error(error_msg);
            return nullptr;
        }

        unique_ptr<data_connection> connection = make_unique<data_connection>();

        connection->open(control_connection_.ip(), port);

        return connection;
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
        return nullptr;
    }
}

/* The text returned in response to the EPSV command MUST be:
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
bool client::try_parse_server_port(const string & epsv_reply, uint16_t & port)
{
    size_t begin = epsv_reply.find('|');
    if (begin == string::npos)
    {
        return false;
    }

    // Skip '|||' characters.
    begin += 3;
    if (begin >= epsv_reply.size())
    {
        return false;
    }

    size_t end = epsv_reply.rfind('|');
    if (end == string::npos)
    {
        return false;
    }

    if (end <= begin)
    {
        return false;
    }

    string port_str = epsv_reply.substr(begin, end - begin);

    return boost::conversion::try_lexical_convert(port_str, port);
}

void client::handle_connection_exception(const connection_exception & ex)
{
    string error_msg = ex.what();

    notify_of_error(error_msg);

    control_connection_.reset();
}

void client::subscribe(event_observer *observer)
{
    observers_.push_back(observer);
}

void client::unsubscribe(event_observer *observer)
{
    observers_.remove(observer);
}

void client::notify_of_reply(const string & reply)
{
    for (const auto & observer : observers_)
        observer->on_reply(reply);
}

void client::notify_of_reply(const reply_t & reply)
{
    for (const auto & observer : observers_)
        observer->on_reply(reply.status_line);
}

void client::notify_of_error(const string & error)
{
    for (const auto & observer : observers_)
        observer->on_error(error);
}

} // namespace ftp
