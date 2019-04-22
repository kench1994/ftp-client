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

#ifndef FTP_CLIENT_DATA_TRANSFER_MODE_HPP
#define FTP_CLIENT_DATA_TRANSFER_MODE_HPP

#include "control_connection.hpp"
#include "data_connection.hpp"

namespace ftp
{

/**
 * The data_transfer_mode class implements the strategy design pattern for determining of data transfer mode.
 */
class data_transfer_mode
{
public:
    explicit data_transfer_mode(boost::asio::io_context & io_context);

    virtual std::unique_ptr<data_connection> open_data_connection(control_connection & control_connection) = 0;

    virtual ~data_transfer_mode() = default;

protected:
    boost::asio::io_context & io_context_;
};

class passive_mode : public data_transfer_mode
{
public:
    explicit passive_mode(boost::asio::io_context & io_context);

    std::unique_ptr<data_connection> open_data_connection(control_connection & control_connection) override;

private:
    boost::asio::ip::tcp::endpoint get_endpoint_from_server_reply(const std::string & reply);
};

} // namespace ftp
#endif //FTP_CLIENT_DATA_TRANSFER_MODE_HPP
