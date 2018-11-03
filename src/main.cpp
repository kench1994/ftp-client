#include <iostream>

#include "ftp_client.hpp"
#include "user_interface.hpp"

using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
    user_interface ui;
    ui.run();

    return 0;
}
