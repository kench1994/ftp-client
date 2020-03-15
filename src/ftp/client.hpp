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

#ifndef FTP_CLIENT_HPP
#define FTP_CLIENT_HPP

#include "detail/control_connection.hpp"
#include "detail/data_connection.hpp"
#include "detail/connection_exception.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <list>

namespace ftp
{

class client
{
public:
    client() = default;

    client(const client &) = delete;

    client & operator=(const client &) = delete;

    void open(const std::string & hostname, uint16_t port = 21);

    bool is_open() const;

    void user(const std::string & username, const std::string & password);

    void cwd(const std::string & remote_directory);

    void list(const std::optional<std::string> & remote_directory = std::nullopt);

    void stor(const std::string & local_file, const std::string & remote_file);

    void retr(const std::string & remote_file, const std::string & local_file);

    void pwd();

    void mkd(const std::string & directory_name);

    void rmd(const std::string & directory_name);

    void dele(const std::string & remote_file);

    void type_i();

    void size(const std::string & remote_file);

    void stat(const std::optional<std::string> & remote_file = std::nullopt);

    void syst();

    void noop();

    void quit();

    class event_observer
    {
    public:
        virtual void on_reply(const std::string & reply) = 0;

        virtual void on_error(const std::string & error) = 0;

        virtual ~event_observer() = default;
    };

    void subscribe(event_observer *observer);

    void unsubscribe(event_observer *observer);

private:
    std::unique_ptr<detail::data_connection> create_data_connection();

    static bool try_parse_server_port(const std::string & epsv_reply, uint16_t & port);

    void handle_connection_exception(const detail::connection_exception & ex);

    void report_reply(const std::string & reply);

    void report_reply(const detail::reply_t & reply);

    void report_error(const std::string & error);

    detail::control_connection control_connection_;
    std::list<event_observer *> observers_;
    std::array<char, 4096> buffer_;
};

} // namespace ftp
#endif //FTP_CLIENT_HPP
