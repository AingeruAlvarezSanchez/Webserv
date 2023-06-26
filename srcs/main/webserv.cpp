#include <iostream>
#include <vector>
#include "webserv.h"
#include "server_info.hpp"

int main(int argc, char **argv) {
    if (argc > 2) {
        errno = E2BIG;
        std::cerr << "Webserv: could not execute: " << strerror(errno) << "\n";
        return 1;
    } else if (argc < 2 || !isValidFile(argv[1])) {
        errno = EINVAL;
        std::cerr << "Webserv: could not execute: " << strerror(errno) << "\n";
        return 1;
    }

    try {
        std::cout << "---------- Start of parse ----------\n";
        ServerInfo const serverInfo(argv[1]);
        std::cout << "----------- End of parse -----------\n";
        //TODO testing getters
        std::cout << "port>" << serverInfo.getServerPort(0) << "<port\n";
        std::cout << "server_name0>" << serverInfo.getServerNames(0)[0] << "<server_name0\n";
        std::cout << "server_name1>" << serverInfo.getServerNames(0)[1] << "<server_name1\n";
        std::cout << "allowed_host0>" << serverInfo.getServerHosts(0)[0] << "<allowed_host0\n";
        std::cout << "max_bytes>" << serverInfo.getServerMaxBytes(0) << "<max_bytes\n";
        std::cout << "errorPageRoutesNb>" << serverInfo.getServerErrorPageRoutes(0, 404)->second[0] << "<errorPageRoutesNb\n";
        //TODO testing getters
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
