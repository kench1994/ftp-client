#include <iostream>

#include "ftp_client.hpp"

using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: ./ftp_client <hostname> <port>" << endl;
        return EXIT_FAILURE;
    }

    ftp::client client(argv[1], static_cast<unsigned short>(atoi(argv[2])));

    return 0;
}
