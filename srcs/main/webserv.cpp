#include <iostream>
#include <cstring>
#include "webserv.h"
#include "../classes/server_conf.hpp" //TODO

int check_arguments(int argc, const std::string &file) { //TODO file
    if (argc < 2) {
        errno = EINVAL;
        return -1;
    } else if (argc > 2) {
        errno = E2BIG;
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (check_arguments(argc, argv[1]) == -1) {
        std::cerr << "Webserv: " << strerror(errno) << "\n";
        return 1;
    }

    ServerConf serverConf;
    try {
        std::string content = config_file_content(argv[1]);
        if (config_file_parser(content, serverConf) == -1) {
            std::cerr << "Webserv: Bad syntax\n"; //TODO
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    //////////////////////TODO///////////////////////////
    /*for (auto it : serverConf) {
        std::cout << "Port: " << it.server().port << "\n";
        std::cout << "Host: " << it.server().ipv4Host.s_addr << "\n";
        std::cout << "names: ";
        for (auto it2 : it.server().servNames) {
            std::cout << it2 << "\n";
        }
    }*/
    //////////////////////TODO//////////////////////////
    //start();

    return 0;
}
