#include <fstream>
#include "webserv.h"

bool conf_file_parsing(std::string const &fileName) {
    std::ifstream file(fileName.c_str());

    if (file.good()) {
        std::string line;
        while (std::getline(file, line)) {
            //TODO tratar aqui la linea
            std::cout << "//" << line << "//\n";
        }
        return 0;
    } else {
        throw std::runtime_error("Webserv: runtime error: ");
    }
}
