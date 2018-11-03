#ifndef FTP_CLIENT_REQUEST_PARSER_HPP
#define FTP_CLIENT_REQUEST_PARSER_HPP

#include <vector>
#include <string>

using std::vector;
using std::string;

namespace ftp
{

class request_parser
{
public:
    static vector<string> parse(const string & request);
};

} // namespace ftp
#endif //FTP_CLIENT_REQUEST_PARSER_HPP
