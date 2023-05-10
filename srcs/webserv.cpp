#include <cstring>
#include <fstream>
#include "webserv.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        errno = 22;
        std::cout << "Webserv: could not execute: " << strerror(errno) << "\n";
        return 1;
    } else if (argc > 2) {
        errno = 7;
        std::cout << "Webserv: could not execute: " << strerror(errno) << "\n";
        return 1;
    }

    try {
        conf_file_parsing(argv[1]);
    }
    catch (std::exception& e) {
        std::cout << e.what() << strerror(errno) << "\n";
        return 1;
    }
    return 0;
}
