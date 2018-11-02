#include "ftp_client.hpp"

namespace ftp
{

client::client(const string & hostname, unsigned short port)
        : hostname_(hostname),
          port_(port)
{
}

} // namespace ftp
