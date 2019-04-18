/**
 * client.cpp
 *
 * Copyright (c) 2018, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
 */

#include "client.hpp"
#include "resources.hpp"
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
    cout << control_connection_->read() << endl;
}

bool client::is_open() const
{
    return control_connection_ != nullptr;
}

void client::user(const string & username)
{
    control_connection_->write(command::remote::user + " " + username);
    cout << control_connection_->read() << endl;
}

void client::pass(const string & password)
{
    control_connection_->write(command::remote::password + " " + password);
    cout << control_connection_->read() << endl;
}

void client::cd(const string & remote_directory)
{
    control_connection_->write(command::remote::cd + " " + remote_directory);
    cout << control_connection_->read() << endl;
}

void client::list(const optional<string> & remote_directory)
{
    string command = command::remote::ls;

    if (remote_directory)
    {
        command += " " + remote_directory.value();
    }

    unique_ptr<data_connection> data_connection =
            data_transfer_mode_->open_data_connection(*control_connection_);

    control_connection_->write(command);
    cout <<  control_connection_->read() << endl;

    cout << data_connection->read() << endl;
    // Don't keep the data connection.
    data_connection.reset();

    cout << control_connection_->read() << endl;
}

void client::get(const string & remote_file, ofstream & file)
{
    unique_ptr<data_connection> data_connection =
            data_transfer_mode_->open_data_connection(*control_connection_);

    control_connection_->write(command::remote::get + " " + remote_file);
    cout << control_connection_->read() << endl;

    data_connection->read_file(file);
    // Don't keep the data connection.
    data_connection.reset();

    cout << control_connection_->read() << endl;
}

void client::pwd()
{
    control_connection_->write(command::remote::pwd);
    cout << control_connection_->read() << endl;
}

void client::mkdir(const string & directory_name)
{
    control_connection_->write(command::remote::mkdir + " " + directory_name);
    cout << control_connection_->read() << endl;
}

void client::ascii()
{
    control_connection_->write(command::remote::ascii);
    cout << control_connection_->read() << endl;
}

void client::binary()
{
    control_connection_->write(command::remote::binary);
    cout << control_connection_->read() << endl;
}

void client::size(const string & remote_file)
{
    control_connection_->write(command::remote::size + " " + remote_file);
    cout << control_connection_->read() << endl;
}

void client::syst()
{
    control_connection_->write(command::remote::syst);
    cout << control_connection_->read() << endl;
}

void client::close()
{
    control_connection_->write(command::remote::close);
    cout << control_connection_->read() << endl;
    control_connection_.reset();
    data_transfer_mode_.reset();
}

void client::reset()
{
    control_connection_.reset();
    data_transfer_mode_.reset();
}

} // namespace ftp
