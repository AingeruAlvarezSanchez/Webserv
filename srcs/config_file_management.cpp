#include <fstream>
#include <iostream>
#include "classes/server_conf.hpp"
#include "webserv.h"

bool conf_file_parsing(std::string const& fileName) {
    std::ifstream file(fileName.c_str());

    if (file.good()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("http") != std::string::npos) {
                std::cout << "http directive found\n";
                /* The reason there is an else if is that there cant be a server
                 * directive outside a http directive, so I will handle here the server,
                 * and else if means it outside */
                //TODO server must be inside of http
                //TODO location must be inside of server
                //TODO where can cgi be located?
                return 0;
            } else if (line.find("server")) {
                /*TODO I will change this so the error can receive every other error,
                 * not only server, thus erasing the else if forest*/
                errno = 134;
                throw ServerConf::BadSyntax("Webserv: syntax error: server directive is not allowed here");
            }
        }
    } else {
        throw std::runtime_error("Webserv: runtime error: ");
    }
    return 1;
}
