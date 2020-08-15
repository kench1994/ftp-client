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
using std::pair;
using std::make_pair;
using std::nullopt;
using std::make_optional;
using std::exception;

using namespace ftp::detail;

command_result client::open(const string & hostname, uint16_t port)
{
    try
    {
        open_connection(hostname, port);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

bool client::is_open() const
{
    return control_connection_.is_open();
}

command_result client::login(const string & username, const string & password)
{
    try
    {
        send("USER " + username);

        reply_t reply = recv();

        if (reply.status_code == 331)
        {
            /* 331 User name okay, need password. */
            send("PASS " + password);

            reply = recv();
        }
        else if (reply.status_code == 332)
        {
            /* 332 Need account for login.
             * Sorry, we don't support ACCT command.
             */
        }

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::cd(const string & remote_directory)
{
    try
    {
        send("CWD " + remote_directory);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::ls(const optional<string> & remote_directory)
{
    try
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

        optional<data_connection> data_connection = initiate_data_connection();

        if (!data_connection)
        {
            return command_result::not_ok;
        }

        data_connection->open();

        send(command);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }

        string file_list = data_connection->recv();
        report_reply(file_list);

        /* Don't keep the data connection. */
        data_connection->close();

        reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::upload(const string & local_file, const string & remote_file)
{
    try
    {
        ifstream file(local_file, ios_base::binary);

        if (!file)
        {
            string error_msg = utils::format("Cannot open file %1%.", local_file);
            report_error(error_msg);
            return command_result::not_ok;
        }

        optional<data_connection> data_connection = initiate_data_connection();

        if (!data_connection)
        {
            return command_result::not_ok;
        }

        data_connection->open();

        send("STOR " + remote_file);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }

        data_connection->send(file);

        /* Don't keep the data connection. */
        data_connection->close();

        reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::download(const string & remote_file, const string & local_file)
{
    try
    {
        if (std::filesystem::exists(local_file))
        {
            string error_msg = utils::format("The file '%1%' already exists.", local_file);
            report_error(error_msg);
            return command_result::not_ok;
        }

        ofstream file(local_file, ios_base::binary);

        if (!file)
        {
            string error_msg = utils::format("Cannot create file %1%.", local_file);
            report_error(error_msg);
            return command_result::not_ok;
        }

        optional<data_connection> data_connection = initiate_data_connection();

        if (!data_connection)
        {
            return command_result::not_ok;
        }

        data_connection->open();

        send("RETR " + remote_file);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }

        data_connection->recv(file);

        /* Don't keep the data connection. */
        data_connection->close();

        reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::pwd()
{
    try
    {
        send("PWD");

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::mkdir(const string & directory_name)
{
    try
    {
        send("MKD " + directory_name);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::rmdir(const string & directory_name)
{
    try
    {
        send("RMD " + directory_name);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::rm(const string & remote_file)
{
    try
    {
        send("DELE " + remote_file);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::binary()
{
    try
    {
        send("TYPE I");

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::size(const string & remote_file)
{
    try
    {
        send("SIZE " + remote_file);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::stat(const optional<string> & remote_file)
{
    try
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

        send(command);

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::system()
{
    try
    {
        send("SYST");

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::noop()
{
    try
    {
        send("NOOP");

        reply_t reply = recv();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

command_result client::close()
{
    try
    {
        send("QUIT");

        reply_t reply = recv();

        close_connection();

        if (reply.is_negative())
        {
            return command_result::not_ok;
        }
        else
        {
            return command_result::ok;
        }
    }
    catch (const exception & ex)
    {
        reset_connection();
        report_exception(ex);
        return command_result::error;
    }
    catch (...)
    {
        reset_connection();
        return command_result::error;
    }
}

void client::open_connection(const string & hostname, uint16_t port)
{
    control_connection_.open(hostname, port);
}

void client::close_connection()
{
    control_connection_.close();
}

void client::send(const string & command)
{
    control_connection_.send(command);
}

reply_t client::recv()
{
    reply_t reply = control_connection_.recv();

    report_reply(reply);

    return reply;
}

void client::reset_connection()
{
    control_connection_.reset();
}

optional<data_connection> client::initiate_data_connection()
{
    send("EPSV");

    reply_t reply = recv();

    if (reply.is_negative())
    {
        return nullopt;
    }

    uint16_t port;
    if (!try_parse_server_port(reply.status_line, port))
    {
        throw connection_exception("Cannot parse server port from '%1%'.", reply.status_line);
    }

    return make_optional<data_connection>(control_connection_.ip(), port);
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

void client::subscribe(event_observer *observer)
{
    observers_.push_back(observer);
}

void client::unsubscribe(event_observer *observer)
{
    observers_.remove(observer);
}

void client::report_exception(const exception & ex)
{
    string error_msg = ex.what();

    report_error(error_msg);
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
