#include <iostream>
#include <cstring>
#include "webserv.h"
#include "server.hpp"

int main(int argc, char **argv) {
    if (!argv[1] || check_arguments(argc, argv[1]) == -1) {
        std::cerr << "Webserv: " << strerror(errno) << "\n";
        return 1;
    }

    std::vector<ServerConf> serverConf;
    try {
        std::string content = config_file_content(argv[1]);
        if (config_file_parser(content, serverConf) == -1) {
            std::cerr << "Webserv: Bad syntax\n";
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    for (std::vector<ServerConf>::iterator it = serverConf.begin(); it != serverConf.end(); it++) {
        std::cout << *it;
    }

    SocketManager socketMan(serverConf.begin(), serverConf.end(), SOCK_STREAM);
    Server server;
    server.start(socketMan);

    return 0;
}
