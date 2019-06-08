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
#include <iostream>

namespace ftp
{

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::make_unique;
using std::optional;
using std::ofstream;
using std::unique_ptr;

void client::open(const string & hostname, const string & port)
{
    data_transfer_mode_ = make_unique<passive_mode>(io_context_);
    control_connection_ = make_unique<control_connection>(io_context_, hostname, port);
    cout << control_connection_->recv() << endl;
}

bool client::is_open() const
{
    return control_connection_ != nullptr;
}

void client::user(const string & username)
{
    control_connection_->send("USER " + username);
    cout << control_connection_->recv() << endl;
}

void client::pass(const string & password)
{
    control_connection_->send("PASS " + password);
    cout << control_connection_->recv() << endl;
}

void client::cd(const string & remote_directory)
{
    control_connection_->send("CWD " + remote_directory);
    cout << control_connection_->recv() << endl;
}

void client::ls(const optional<string> & remote_directory)
{
    string command = "LIST";

    if (remote_directory)
    {
        command += ' ' + remote_directory.value();
    }

    unique_ptr<data_connection> data_connection =
            data_transfer_mode_->open_data_connection(*control_connection_);

    control_connection_->send(command);
    cout <<  control_connection_->recv() << endl;

    cout << data_connection->recv() << endl;
    // Don't keep the data connection.
    data_connection.reset();

    cout << control_connection_->recv() << endl;
}

void client::get(const string & remote_file, ofstream & file)
{
    unique_ptr<data_connection> data_connection =
            data_transfer_mode_->open_data_connection(*control_connection_);

    control_connection_->send("RETR " + remote_file);
    cout << control_connection_->recv() << endl;

    data_connection->recv_file(file);
    // Don't keep the data connection.
    data_connection.reset();

    cout << control_connection_->recv() << endl;
}

void client::pwd()
{
    control_connection_->send("PWD");
    cout << control_connection_->recv() << endl;
}

void client::mkdir(const string & directory_name)
{
    control_connection_->send("MKD " + directory_name);
    cout << control_connection_->recv() << endl;
}

void client::ascii()
{
    control_connection_->send("TYPE A");
    cout << control_connection_->recv() << endl;
}

void client::binary()
{
    control_connection_->send("TYPE I");
    cout << control_connection_->recv() << endl;
}

void client::size(const string & remote_file)
{
    control_connection_->send("SIZE " + remote_file);
    cout << control_connection_->recv() << endl;
}

void client::syst()
{
    control_connection_->send("SYST");
    cout << control_connection_->recv() << endl;
}

void client::noop()
{
    control_connection_->send("NOOP");
    cout << control_connection_->recv() << endl;
}

void client::close()
{
    control_connection_->send("QUIT");
    cout << control_connection_->recv() << endl;
    control_connection_.reset();
    data_transfer_mode_.reset();
}

void client::reset()
{
    control_connection_.reset();
    data_transfer_mode_.reset();
}

} // namespace ftp
