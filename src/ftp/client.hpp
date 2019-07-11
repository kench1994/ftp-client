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

#ifndef FTP_CLIENT_CLIENT_HPP
#define FTP_CLIENT_CLIENT_HPP

#include <string>
#include <vector>
#include "control_connection.hpp"
#include "data_connection.hpp"
#include <fstream>
#include <list>

namespace ftp
{

class client
{
public:
    void open(const std::string & hostname, uint16_t port = 21);

    bool is_open() const;

    void user(const std::string & username, const std::string & password);

    void cd(const std::string & remote_directory);

    void ls(const std::string & remote_directory = {});

    void get(const std::string & remote_file, std::ofstream & file);

    void pwd();

    void mkdir(const std::string & directory_name);

    void binary();

    void size(const std::string & remote_file);

    void syst();

    void noop();

    void close();

    class reply_observer
    {
    public:
        virtual void handle_reply(const std::string & reply) = 0;

        virtual ~reply_observer() = default;
    };

    void add_observer(reply_observer *observer);

    void remove_observer(reply_observer *observer);

private:
    std::unique_ptr<data_connection> create_data_connection();

    uint16_t parse_epsv_port(const std::string & reply);

    void notify_observers(const std::string & reply);

    control_connection control_connection_;
    std::list<reply_observer *> observers_;
};

} // namespace ftp
#endif //FTP_CLIENT_CLIENT_HPP
