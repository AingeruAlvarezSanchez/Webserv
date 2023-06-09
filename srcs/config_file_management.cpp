#include <fstream>
#include <iostream>
#include "classes/server_conf.hpp"
#include "webserv.h"

ServerConf conf_file_parsing(std::string const& fileName) {
    std::ifstream file(fileName.c_str());
    static std::vector< ServerConf::socketParams >  socketParams;

    if (file.good()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("http") != std::string::npos) {
                std::cout << "http directive found\n";
                //TODO fill confValuesStructs

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
    return ServerConf(socketParams);
}
