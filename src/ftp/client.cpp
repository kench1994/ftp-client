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

namespace ftp
{

using std::string;
using std::vector;
using std::make_unique;
using std::optional;
using std::ofstream;
using std::unique_ptr;

client::client()
    : io_context_(),
      control_connection_(io_context_)
{
    data_transfer_mode_ = make_unique<passive_mode>(io_context_);
}

void client::open(const string & hostname, const string & port)
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

void client::ls(const optional<string> & remote_directory)
{
    string command = "LIST";

    if (remote_directory)
    {
        command += ' ' + remote_directory.value();
    }

    unique_ptr<data_connection> data_connection =
            data_transfer_mode_->open_data_connection(control_connection_);

    control_connection_.send(command);
    notify_observers(control_connection_.recv());

    notify_observers(data_connection->recv());
    // Don't keep the data connection.
    data_connection.reset();

    notify_observers(control_connection_.recv());
}

void client::get(const string & remote_file, ofstream & file)
{
    unique_ptr<data_connection> data_connection =
            data_transfer_mode_->open_data_connection(control_connection_);

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
