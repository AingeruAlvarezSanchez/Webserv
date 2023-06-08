#include <cstring>
#include <fstream>
#include <iostream>
#include "webserv.h"
#include "classes/server_conf.hpp"

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
        const ServerConf  serverConf = conf_file_parsing(argv[1]);
        if (serverConf.getSocketConfData(3) != ServerConf::invalidSocket) {
        }
        std::cout << serverConf.getSocketConfData(3)->socketFd << std::endl;
        std::cout << serverConf.getSocketConfData(9000)->socketFd << std::endl;
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
