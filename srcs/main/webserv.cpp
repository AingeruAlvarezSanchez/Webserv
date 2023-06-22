#include <iostream>
#include <vector>
#include "webserv.h"
#include "server_info.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        errno = EINVAL;
        std::cerr << "Webserv: could not execute: " << strerror(errno) << "\n";
        return 1;
    } else if (argc > 2) {
        errno = E2BIG;
        std::cerr << "Webserv: could not execute: " << strerror(errno) << "\n";
        return 1;
    }

    try {
        checkFile(argv[1]);
        std::cout << "---------- Start of parse ----------\n";
        ServerInfo const serverInfo(argv[1]);
        std::cout << "----------- End of parse -----------\n";
    }
    catch (std::exception const& e) {
        if (static_cast<std::string>(strerror(errno)).find("Unknown error") == std::string::npos) {
            std::cerr << e.what() << strerror(errno) << "\n";
        } else {
            std::cerr << e.what() << "\n";
        }
        return 1;
    }
    return 0;
}
