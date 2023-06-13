#include <fstream>
#include "classes/server_info.hpp"

#include <iostream>
bool    config_file_parsing(char const *fileName) {
    std::ifstream   file(fileName);

    if (file.is_open()) {
        std::string buffer;
        std::string validFileContent;

        while (file.good()) {
            std::getline(file, buffer);

            if ((buffer.find_first_not_of(' ') != std::string::npos
                && buffer[buffer.find_first_not_of(' ')] == '#')
                || buffer.empty()) {
                continue;
            }

            validFileContent += buffer;
        }
        //TODO
        for (size_t it = 0; it != validFileContent.length(); it++) {
            std::cout << validFileContent[it];
        }
        std::cout << "\n";
        //TODO
        return false;
    } else {
        throw std::runtime_error("Error: Webserv: ");
    }
}
