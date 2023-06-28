#include "../classes/server_conf.hpp" //TODO
#include <iostream>
#include <cerrno>
#include <cstring>

int main(int argc, char **argv) {
    if (argc < 2) {
        errno = EINVAL;
        std::cerr << "Webserv: " << strerror(errno) << "\n";
    } else if (argc > 2) {
        errno = E2BIG;
        std::cerr << "Webserv: " << strerror(errno) << "\n";
    } //TODO file is valid (extension, empty etc.)

    ServerConf serverConf;
    return 0;
}