/*
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
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
#include "data_transfer_mode.hpp"
#include <optional>
#include <fstream>

namespace ftp
{

class client
{
public:
    void open(const std::string & hostname, const std::string & port = "21");

    bool is_open() const;

    void user(const std::string & username);

    void pass(const std::string & password);

    void cd(const std::string & remote_directory);

    void ls(const std::optional<std::string> & remote_directory = std::nullopt);

    void get(const std::string & remote_file, std::ofstream & file);

    void pwd();

    void mkdir(const std::string & directory_name);

    void ascii();

    void binary();

    void size(const std::string & remote_file);

    void syst();

    void noop();

    void close();

    void reset();

private:
    boost::asio::io_context io_context_;
    std::unique_ptr<control_connection> control_connection_;
    std::unique_ptr<data_transfer_mode> data_transfer_mode_;
};

} // namespace ftp
#endif //FTP_CLIENT_CLIENT_HPP
