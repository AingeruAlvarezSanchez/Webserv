#include <iostream>
#include <cstring>
#include "webserv.h"
#include "server.hpp"

int check_arguments(int argc, const std::string &file) {
    if (argc < 2) {
        errno = EINVAL;
        return -1;
    } else if (argc > 2) {
        errno = E2BIG;
        return -1;
    }
    return 0;
} //TODO test different arguments such as invalid extensions.

int main(int argc, char **argv) {
    if (!argv[1] || check_arguments(argc, argv[1]) == -1) {
        std::cerr << "Webserv: " << strerror(errno) << "\n";
        return 1;
    }

    std::vector<ServerConf> serverConf;
    try {
        std::string content = config_file_content(argv[1]);
        if (config_file_parser(content, serverConf) == -1) {
            std::cerr << "Webserv: Bad syntax\n"; //TODO create dynamic messages telling the string that failed.
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    for (const auto &it : serverConf) {
        std::cout << it;
    } //TODO erase this for loop, it displays the current server configuration

    SocketManager socketMan(serverConf.begin(), serverConf.end(), SOCK_STREAM);
    Server server;
    server.start(socketMan);

    return 0;
}
