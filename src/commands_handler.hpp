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

#ifndef FTP_CLIENT_COMMANDS_HANDLER_HPP
#define FTP_CLIENT_COMMANDS_HANDLER_HPP

#include <string>
#include <vector>
#include <iostream>
#include "ftp/client.hpp"

class commands_handler
{
public:
    commands_handler();

    void execute(const std::string & command,
                 const std::vector<std::string> & args);

private:
    bool is_needed_connection(const std::string & command) const;

    void open(const std::vector<std::string> & args);

    void user(const std::vector<std::string> & args);

    void cd(const std::vector<std::string> & args);

    void ls(const std::vector<std::string> & args);

    void get(const std::vector<std::string> & args);

    void pwd();

    void mkdir(const std::vector<std::string> & args);

    void binary();

    void size(const std::vector<std::string> & args);

    void syst();

    void noop();

    void close();

    void help();

    void exit();

    class stdout_writer : public ftp::client::event_observer
    {
    public:
        void on_reply(const std::string & reply) override
        {
            std::cout << reply;
            std::cout.flush();
        }

        void on_error(const std::string & error) override
        {
            std::cout << error << std::endl;
        }
    };

    stdout_writer stdout_writer_;
    ftp::client client_;
};

#endif //FTP_CLIENT_COMMANDS_HANDLER_HPP
