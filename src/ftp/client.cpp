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
#include <fstream>
#include <boost/lexical_cast.hpp>

namespace ftp
{
using std::string;
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
        control_connection_.open(hostname, port);

        reply_t reply = control_connection_.recv();

        report_reply(reply);
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
        control_connection_.send("USER " + username);

        reply_t reply = control_connection_.recv();

        report_reply(reply);

        if (reply.status_code == 331)
        {
            /* 331 User name okay, need password. */
            control_connection_.send("PASS " + password);
            report_reply(control_connection_.recv());
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

void client::cd(const string & remote_directory)
{
    try
    {
        control_connection_.send("CWD " + remote_directory);

        reply_t reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::ls(const optional<string> & remote_directory)
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

        control_connection_.send(command);

        reply_t reply = control_connection_.recv();

        report_reply(reply);

        if (reply.status_code >= 400)
        {
            return;
        }

        report_reply(data_connection->recv());

        /* Don't keep the data connection. */
        data_connection->close();

        reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::upload(const string & local_file, const string & remote_file)
{
    ifstream file(local_file, ios_base::binary);

    if (!file)
    {
        string error_msg = utils::format("Cannot open file %1%.", local_file);
        report_error(error_msg);
        return;
    }

    try
    {
        unique_ptr<data_connection> data_connection = create_data_connection();

        if (!data_connection)
        {
            return;
        }

        control_connection_.send("STOR " + remote_file);

        reply_t reply = control_connection_.recv();

        report_reply(reply);

        if (reply.status_code >= 400)
        {
            return;
        }

        /* Start file transfer. */
        for (;;)
        {
            file.read(buffer_.data(), buffer_.size());

            if (file.fail() && !file.eof())
            {
                report_error("Cannot read data from file.");
                return;
            }

            data_connection->send(buffer_.data(), file.gcount());

            if (file.eof())
                break;
        }

        /* Don't keep the data connection. */
        data_connection->close();

        reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::download(const string & remote_file, const string & local_file)
{
    ofstream file(local_file, ios_base::binary);

    if (!file)
    {
        string error_msg = utils::format("Cannot create file %1%.", local_file);
        report_error(error_msg);
        return;
    }

    try
    {
        unique_ptr<data_connection> data_connection = create_data_connection();

        if (!data_connection)
        {
            return;
        }

        control_connection_.send("RETR " + remote_file);

        reply_t reply = control_connection_.recv();

        report_reply(reply);

        if (reply.status_code >= 400)
        {
            return;
        }

        /* Start file transfer. */
        for (;;)
        {
            size_t size = data_connection->recv(buffer_.data(), buffer_.size());

            if (size == 0)
                break;

            file.write(buffer_.data(), size);

            if (file.fail())
            {
                report_error("Cannot write data to file.");
                return;
            }
        }

        /* Don't keep the data connection. */
        data_connection->close();

        reply = control_connection_.recv();

        report_reply(reply);
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
        control_connection_.send("PWD");

        reply_t reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::mkdir(const string & directory_name)
{
    try
    {
        control_connection_.send("MKD " + directory_name);

        reply_t reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::rmdir(const string & directory_name)
{
    try
    {
        control_connection_.send("RMD " + directory_name);

        reply_t reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::rm(const string & remote_file)
{
    try
    {
        control_connection_.send("DELE " + remote_file);

        reply_t reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::binary()
{
    try
    {
        control_connection_.send("TYPE I");

        reply_t reply = control_connection_.recv();

        report_reply(reply);
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
        control_connection_.send("SIZE " + remote_file);

        reply_t reply = control_connection_.recv();

        report_reply(reply);
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
        control_connection_.send(command);

        reply_t reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::system()
{
    try
    {
        control_connection_.send("SYST");

        reply_t reply = control_connection_.recv();

        report_reply(reply);
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
        control_connection_.send("NOOP");

        reply_t reply = control_connection_.recv();

        report_reply(reply);
    }
    catch (const connection_exception & ex)
    {
        handle_connection_exception(ex);
    }
}

void client::close()
{
    try
    {
        control_connection_.send("QUIT");

        reply_t reply = control_connection_.recv();

        report_reply(reply);

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
        control_connection_.send("EPSV");

        reply_t reply = control_connection_.recv();

        report_reply(reply);

        if (reply.status_code >= 400)
        {
            return nullptr;
        }

        uint16_t port;
        if (!try_parse_server_port(reply.status_line, port))
        {
            string error_msg = utils::format("Cannot parse server port from '%1%'.",
                                             reply.status_line);
            report_error(error_msg);
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

    /* Skip '|||' characters. */
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

    report_error(error_msg);

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

void client::report_reply(const string & reply)
{
    for (const auto & observer : observers_)
    {
        if (observer)
            observer->on_reply(reply);
    }
}

void client::report_reply(const reply_t & reply)
{
    for (const auto & observer : observers_)
    {
        if (observer)
            observer->on_reply(reply.status_line);
    }
}

void client::report_error(const string & error)
{
    for (const auto & observer : observers_)
    {
        if (observer)
            observer->on_error(error);
    }
}

} // namespace ftp
