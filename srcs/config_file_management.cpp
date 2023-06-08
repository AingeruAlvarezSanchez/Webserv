#include <fstream>
#include <iostream>
#include "classes/server_conf.hpp"
#include "webserv.h"

ServerConf const& conf_file_parsing(std::string const& fileName) {
    std::ifstream file(fileName.c_str());
    std::vector< ServerConf::confValuesData >  confValuesStructs;
    ServerConf::confValuesData config;
    ServerConf::confValuesData config2;

    config.socketFd = 3;

    config2.socketFd = 4;

    confValuesStructs.push_back(config);
    confValuesStructs.push_back(config2);
    if (file.good()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("http") != std::string::npos) {
                std::cout << "http directive found\n";
                //TODO fill the struct with info

                //TODO server must be inside of http
                //TODO location must be inside of server
                //TODO where can cgi be located?
            } else if (!line.empty()) {
                errno = ESYNTAX;
                throw ServerConf::BadSyntax("Webserv: syntax error: server directive is not allowed here"); //TODO temporary, can be any error not just server, but want to handle every error properly instead of generic "syntax error"
            }
        }
    } else {
        throw std::runtime_error("Webserv: runtime error: ");
    }
    return *(new const ServerConf(confValuesStructs));
}
