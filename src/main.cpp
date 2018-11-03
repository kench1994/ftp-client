#include <iostream>

#include "ftp_client.hpp"
#include "user_interface.hpp"

using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: ./ftp_client <hostname> <port>" << endl;
        return EXIT_FAILURE;
    }

    user_interface ui;
    ui.run();

    return 0;
}
