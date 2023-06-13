#include <fstream>
#include <cstring>
#include "classes/server_info.hpp"

std::string fill_server_data(std::string configContent) {
    size_t  firstNonSpace = configContent.find_first_not_of(' ');
    size_t  serverDirectiveStart = configContent.find("server");

    if (configContent[firstNonSpace] == configContent[serverDirectiveStart]) {
        std::string serverDirectiveContent("server");
        configContent.erase(0, std::strlen("server"));

        size_t  apertureBrace = configContent.find_first_of('{');
        firstNonSpace = configContent.find_first_not_of(' ');

        if (configContent[firstNonSpace] == configContent[apertureBrace]) {
            serverDirectiveContent.append(configContent, 0, (configContent.find_first_of('}') + 1)); //TODO maybe overflow if } its just before EOF
        } else {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
        }

        return serverDirectiveContent;
    } else {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
}

#include <iostream>
bool    config_file_parsing(char const *fileName) {
    std::ifstream   file(fileName);

    if (file.is_open()) {
        std::string buffer;
        std::string configContent;

        while (file.good()) {
            std::getline(file, buffer);

            size_t firstNonSpace = buffer.find_first_not_of(' ');
            if ((buffer.find_first_not_of(' ') != std::string::npos && buffer[firstNonSpace] == '#')
                || buffer.empty()) {
                continue;
            }

            configContent += buffer; //TODO "\n" is temporary, just like her love, but buffer is forever
        }

        //TODO CHECKING
        for (size_t it = 0; it != configContent.length(); it++) {
            std::cout << configContent[it];
        }
        std::cout << "\n";
        //TODO CHECKING

        while (!configContent.empty()) {
            std::string serverDirective = fill_server_data(configContent);

            configContent.erase(0, serverDirective.length());
        }

        return false;
    } else {
        throw std::runtime_error("Error: Webserv: ");
    }
}
