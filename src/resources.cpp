#include "resources.hpp"

namespace ftp
{
namespace user_request
{
    const char * open = "open";
    const char * exit = "exit";
} // namespace user_request

namespace error
{
    const char * invalid_request =
        "Invalid request. Use 'help' to display list of FTP commands.";
} // namespace error

namespace usage
{
    const char * open = "Usage: open <hostname> <port>";
} // namespace usage
} // namespace ftp
