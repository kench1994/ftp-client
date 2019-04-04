/**
 * data_transfer_mode.hpp
 *
 * Copyright (c) 2019, Denis Kovalchuk <deniskovjob@gmail.com>
 *
 * This code is licensed under a MIT-style license.
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
    virtual std::unique_ptr<data_connection> activate(control_connection & control_connection) = 0;

    virtual ~data_transfer_mode() = default;
};

class passive_mode : public data_transfer_mode
{
public:
    std::unique_ptr<data_connection> activate(control_connection & control_connection) override;

private:
    boost::asio::ip::tcp::endpoint get_endpoint_from_server_reply(const std::string & reply);
};

} // namespace ftp
#endif //FTP_CLIENT_DATA_TRANSFER_MODE_HPP
