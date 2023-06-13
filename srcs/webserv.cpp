#include <cstring>
#include <cerrno>
#include <iostream>
#include "webserv.h"

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
        std::cout << "Before parsing:\nFile -> " << argv[1] << "\n";
        config_file_parsing(argv[1]);
        std::cout << "Parsing done.\n";
    }
    catch (std::exception const& e) {
        if (static_cast<std::string>(strerror(errno)).find("Unknown error") != std::string::npos) {
            std::cerr << e.what() << "\n";
        } else {
            std::cerr << e.what() << strerror(errno) << "\n";
        }
        return 1;
    }
    return 0;
}
