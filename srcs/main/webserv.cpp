#include <iostream>
#include <cstring>
#include "webserv.h"

int check_arguments(int argc, const std::string &file) { //TODO newf for checking file
    if (argc < 2) {
        errno = EINVAL;
        return -1;
    } else if (argc > 2) {
        errno = E2BIG;
        return -1;
    }
    return 0;
}

#include "../classes/socket_manager.hpp" //TODO
void start(std::vector<ServerConf> &serverConf) {
    SocketManager socketMan(serverConf.begin(), serverConf.end(), SOCK_STREAM);

    std::vector<int> res = socketMan.listenOnSock(socketMan.sockBegin(), socketMan.sockEnd());

    //TODO
    for (auto it : res) {
        std::cout << "fd: " << it << "\n";
    }
    //TODO
}

int main(int argc, char **argv) {
    if (check_arguments(argc, argv[1]) == -1) {
        std::cerr << "Webserv: " << strerror(errno) << "\n";
        return 1;
    }

    std::vector<ServerConf> serverConf;
    try {
        std::string content = config_file_content(argv[1]);
        if (config_file_parser(content, serverConf) == -1) {
            std::cerr << "Webserv: Bad syntax\n"; //TODO better messages?
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    //TODO testing
    for (auto it : serverConf) {
        std::cout << it;
    }
    //TODO testing
    start(serverConf);

    return 0;
}
