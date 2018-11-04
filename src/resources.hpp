/**
 * MIT License
 *
 * Copyright (c) 2018 Denis Kovalchuk
 */

#ifndef FTP_CLIENT_RESOURCES_HPP
#define FTP_CLIENT_RESOURCES_HPP

#include <string>

using std::string;

namespace ftp
{
namespace user_request
{
    const char * open = "open";
    const char * exit = "exit";

} // namespace request

namespace error
{
    const char * invalid_request =
            "Invalid request. Use 'help' to display list of FTP commands.";

} // namespace error
} // namespace ftp
#endif //FTP_CLIENT_RESOURCES_HPP
