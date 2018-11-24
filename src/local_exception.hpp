#ifndef FTP_CLIENT_LOCAL_EXCEPTION_HPP
#define FTP_CLIENT_LOCAL_EXCEPTION_HPP

#include <stdexcept>

using std::runtime_error;

namespace ftp
{

class local_exception : public runtime_error
{
public:
    explicit local_exception(const char * message)
            : runtime_error(message)
    {
    }
};

} // namespace ftp
#endif //FTP_CLIENT_LOCAL_EXCEPTION_HPP
