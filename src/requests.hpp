/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#ifndef FTP_CLIENT_REQUESTS_HPP
#define FTP_CLIENT_REQUESTS_HPP

#include <string>

using std::string;

namespace ftp
{
namespace request
{
    const string open = "open";
    const string exit = "exit";

} // namespace request

namespace error
{
    const string invalid_request =
            "Invalid request. Use 'help' to display list of FTP commands.";

} // namespace error
} // namespace ftp
#endif //FTP_CLIENT_REQUESTS_HPP
