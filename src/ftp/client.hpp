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
#include <string>
#include <list>

namespace ftp
{

class client
{
public:
    class event_observer
    {
    public:
        virtual void on_reply(const std::string & reply) = 0;

        virtual ~event_observer() = default;
    };

    explicit client(client::event_observer *observer = nullptr);

    client(const client &) = delete;

    client & operator=(const client &) = delete;

    bool open(const std::string & hostname, uint16_t port = 21);

    bool is_open();

    bool login(const std::string & username, const std::string & password);

    bool cd(const std::string & remote_directory);

    bool ls(const std::optional<std::string> & remote_directory = std::nullopt);

    bool upload(const std::string & local_file, const std::string & remote_file);

    bool upload_cache(detail::data_connection* pDataConn, const char* pszBuffer, std::size_t uBufferSize);

    bool download(const std::string & remote_file, const std::string & local_file);

    bool pwd();

    bool mkdir(const std::string & directory_name);

    bool rmdir(const std::string & directory_name);

    bool rm(const std::string & remote_file);

    bool binary();

    bool size(const std::string & remote_file);

    bool stat(const std::optional<std::string> & remote_file = std::nullopt);

    bool system();

    bool noop();

    bool close();

    void subscribe(event_observer *observer);

    void unsubscribe(event_observer *observer);

    std::unique_ptr<detail::data_connection> prepare_upload(const std::string & remote_file);

    detail::reply_t send_command(const std::string & command);

    detail::reply_t send_command_s(const std::string & command, const std::string& args);

private:

    detail::reply_t recv();

    void reset_connection();

    std::unique_ptr<detail::data_connection> establish_data_connection(const std::string & command);

    static bool try_parse_server_port(const std::string & epsv_reply, uint16_t & port);

    void report_reply(const std::string & reply);

    void report_reply(const detail::reply_t & reply);

    detail::control_connection control_connection_;
    std::list<event_observer *> observers_;
};

} // namespace ftp
#endif //FTP_CLIENT_HPP
