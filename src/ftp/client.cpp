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
#include "ftp_exception.hpp"
#include "detail/connection_exception.hpp"
#include <filesystem>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "utils/RC4.h"
#include <iostream>

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

using namespace ftp::detail;

client::client(client::event_observer *observer)
{
    if (observer)
    {
        observers_.push_back(observer);
    }
}

bool client::open(const string & hostname, uint16_t port)
{
    try
    {
        control_connection_.open(hostname, port);

        reply_t reply = recv();

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::open_v6(const string & hostname, uint16_t port)
{
    try
    {
        control_connection_.open_v6(hostname, port);

        reply_t reply = recv();

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}


bool client::is_open()
{
    try
    {
        return control_connection_.is_open();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::login(const string & username, const string & password)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command_s("USER_S", username);
		{
			std::unique_ptr<char[]> spPlainText(new char[reply.status_line.length()]);
			memset(spPlainText.get(), 0x00, reply.status_line.length());
			RC4DecryptStr(spPlainText.get(), reply.status_line.c_str() + 2, reply.status_line.length() - 3, "tipray", strlen("tipray"));
			std::cout << spPlainText.get() << std::endl;
		}
        // if (reply.status_code == 331)
        {
            /* 331 User name okay, need password. */
            reply = send_command_s("PASS_S", password);
			{
				std::unique_ptr<char[]> spPlainText(new char[reply.status_line.length()]);
				memset(spPlainText.get(), 0x00, reply.status_line.length());
				RC4DecryptStr(spPlainText.get(), reply.status_line.c_str() + 2, reply.status_line.length() - 3, "tipray", strlen("tipray"));
				std::cout << spPlainText.get() << std::endl;
				std::string ss = spPlainText.get();
				auto npos = std::string::npos;
				if(std::string::npos != (npos = ss.find("Token=")))
				{
					token_ = ss.substr(npos + strlen("Token="));
					token_ = token_.substr(0, token_.length() - 1);
					std::cout << "token=" << token_ << std::endl;
				}

			}
        }
        // else if (reply.status_code == 332)
        {
            /* 332 Need account for login.
             * Sorry, we don't support ACCT command.
             */
        }
		
        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::cd(const string & remote_directory)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("CWD " + remote_directory);

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::ls(const optional<string> & remote_directory)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        string command;

        if (remote_directory)
        {
            command = "LIST " + remote_directory.value();
        }
        else
        {
            command = "LIST";
        }

        unique_ptr<data_connection> data_connection = establish_data_connection(command);

        if (!data_connection)
        {
            return false;
        }

        string file_list = data_connection->recv();
        report_reply(file_list);

        /* Don't keep the data connection. */
        data_connection->close();

        reply_t reply = recv();

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::upload(const string & local_file, const string & remote_file)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        ifstream file(local_file, ios_base::binary);

        if (!file)
        {
            throw ftp_exception("Cannot open file '%1%'.", local_file);
        }

        unique_ptr<data_connection> data_connection = establish_data_connection("STOR " + remote_file);

        if (!data_connection)
        {
            return false;
        }

        data_connection->send(file);

        /* Don't keep the data connection. */
        data_connection->close();

        reply_t reply = recv();

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::upload_cache(detail::data_connection* pDataConn, const char* pszBuffer, std::size_t uBufferSize)
{
    try
    {
        if (!is_open() || !pDataConn->is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        pDataConn->send(pszBuffer, uBufferSize);
        //flush??
        pDataConn->close();
        reply_t reply = recv();

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

std::unique_ptr<detail::data_connection> client::prepare_upload(const std::string & remote_file)
{
    if (!is_open())
    {
        throw ftp_exception("Connection is not open.");
    }

    unique_ptr<data_connection> data_connection = establish_data_connection("STOR");// + remote_file);

    if (!data_connection)
    {
        return nullptr;
    }

    return data_connection;
}

bool client::download(const string & remote_file, const string & local_file)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        if (std::filesystem::exists(local_file))
        {
            throw ftp_exception("The file '%1%' already exists.", local_file);
        }

        ofstream file(local_file, ios_base::binary);

        if (!file)
        {
            throw ftp_exception("Cannot create file %1%.", local_file);
        }

        unique_ptr<data_connection> data_connection = establish_data_connection("RETR " + remote_file);

        if (!data_connection)
        {
            return false;
        }

        data_connection->recv(file);

        /* Don't keep the data connection. */
        data_connection->close();

        reply_t reply = recv();

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::pwd()
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("PWD");

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::mkdir(const string & directory_name)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("MKD " + directory_name);

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::rmdir(const string & directory_name)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("RMD " + directory_name);

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::rm(const string & remote_file)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("DELE " + remote_file);

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::binary()
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("TYPE I");

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::size(const string & remote_file)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("SIZE " + remote_file);

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::stat(const optional<string> & remote_file)
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        string command;

        if (remote_file)
        {
            command = "STAT " + remote_file.value();
        }
        else
        {
            command = "STAT";
        }

        reply_t reply = send_command(command);

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::system()
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("SYST");

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::noop()
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("NOOP");

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

bool client::close()
{
    try
    {
        if (!is_open())
        {
            throw ftp_exception("Connection is not open.");
        }

        reply_t reply = send_command("QUIT");

        control_connection_.close();

        return reply.is_positive();
    }
    catch (const connection_exception & ex)
    {
        reset_connection();
        throw ftp_exception(ex);
    }
}

reply_t client::send_command(const string & command)
{
    control_connection_.send(command);

    reply_t reply = control_connection_.recv();

    report_reply(reply);

    return reply;
}

inline bool endWith(const std::string& str, const std::string& cmp) {
    if(!str.length())
        return false;
    if(str.length() < cmp.length())
        return false;
    if(strncmp(str.data() + str.length() - cmp.length(), cmp.data(), cmp.length()))
        return false;
    return true;
}

detail::reply_t client::send_command_s(const std::string & command, const std::string& args)
{
    if(!endWith(command, "_S")) 
	{
		if(args.empty())
		{
			send_command(command);
		}
		else
		{
			std::string buffer = command + " " + args;
			send_command(buffer);
		}
	}
	else
	{
		int nBufSize = command.length() + 10 + args.length() * 2;
		std::unique_ptr<char[]> spBuffer(new char[nBufSize]);
		memset(spBuffer.get(), 0x00, nBufSize);
		int nOffset = sprintf(spBuffer.get(), "%s 20", command.c_str());
		if(!args.empty())
		{
			if(token_.empty())
				RC4EncryptStr(spBuffer.get() + nOffset, args.c_str(), args.length(), "tipray", strlen("tipray"));
			else
				RC4EncryptStr(spBuffer.get() + nOffset, args.c_str(), args.length(), token_.c_str(), token_.length());
		}
		control_connection_.send(std::string(spBuffer.get()));
	}
    reply_t reply = control_connection_.recv();

    report_reply(reply);
    return reply;
}

const std::string & client::getToken()
{
	return token_;
}

reply_t client::recv()
{
    reply_t reply = control_connection_.recv();

    report_reply(reply);

    return reply;
}

void client::reset_connection()
{
    try
    {
        control_connection_.close();
    }
    catch (...)
    {
    }
}

unique_ptr<data_connection> client::establish_data_connection(const string & command)
{
    if (!is_open())
    {
        throw ftp_exception("Connection is not open.");
    }

    reply_t reply = send_command_s("EPSV_S", "");

    // if (!reply.is_positive())
    // {
    //     return nullptr;
    // }

	std::cout << reply.status_line << std::endl;
    std::unique_ptr<char[]> spPlainText(new char[reply.status_line.length()]);
    memset(spPlainText.get(), 0x00, reply.status_line.length());
	RC4DecryptStr(spPlainText.get(), reply.status_line.c_str() + 2, reply.status_line.length() - 3, 
	token_.c_str(), token_.length());
    std::string ss = spPlainText.get();
	std::cout << ss << std::endl;

    uint16_t port;
    if (!try_parse_server_port(ss, port))
    {	
        throw ftp_exception("Cannot parse server port from '%1%'.", reply.status_line);
    }

    unique_ptr<data_connection> connection = make_unique<data_connection>(control_connection_.ip(), port);

    connection->open();

    reply = send_command_s(command, "1.txt");

    if (!reply.is_positive())
    {
        return nullptr;
    }

    return connection;
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

} // namespace ftp
