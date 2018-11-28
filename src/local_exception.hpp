#ifndef FTP_CLIENT_LOCAL_EXCEPTION_HPP
#define FTP_CLIENT_LOCAL_EXCEPTION_HPP

#include <stdexcept>

namespace ftp
{

class local_exception : public std::runtime_error
{
public:
    explicit local_exception(const char * message)
            : std::runtime_error(message)
    {
    }
};

} // namespace ftp
#endif //FTP_CLIENT_LOCAL_EXCEPTION_HPP
