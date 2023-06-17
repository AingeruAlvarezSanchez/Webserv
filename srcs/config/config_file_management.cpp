#include <fstream>
#include <cstring>
#include "webserv.h"

static std::string get_server_directive(std::string configContent) {
    size_t  firstNonSpace = configContent.find_first_not_of(' ');
    size_t  serverDirectiveStart = configContent.find("server");

    if (configContent[firstNonSpace] == configContent[serverDirectiveStart]) {
        std::string serverDirectiveContent("server");
        configContent.erase(0, std::strlen("server"));

        size_t  apertureBrace = configContent.find_first_of('{');
        if (serverDirectiveContent.find_first_of('{') != serverDirectiveContent.find_last_of('{')) {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
        }

        firstNonSpace = configContent.find_first_not_of(' ');
        if (configContent[firstNonSpace] == configContent[apertureBrace]) {
            serverDirectiveContent.append(configContent, 0, (configContent.find_first_of('}') + 1));
        } else {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
        }
        return  serverDirectiveContent;
    } else {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
}

std::string fill_directive_content(std::ifstream & file) {
    std::string buffer;
    std::string directiveContent;

    while (file.good()) {
        std::getline(file, buffer);

        size_t firstNonSpace = buffer.find_first_not_of(' ');
        if ((firstNonSpace != std::string::npos && buffer[firstNonSpace] == '#')
            || buffer.empty()) {
            continue;
        }
        directiveContent += buffer += '\n';
    }
    return directiveContent;
}

ServerInfo  config_file_parsing(char const *fileName) {
    std::ifstream   file(fileName);

    if (file.is_open()) {
        std::string directiveContent = fill_directive_content(file);

        ServerInfo  serverInfo;
        while (!directiveContent.empty()) {
            std::string serverDirective = get_server_directive(directiveContent);
            size_t serverDirectiveLength = serverDirective.length();

            serverInfo.setServerData(get_directive_conf(serverDirective));
            directiveContent.erase(0, serverDirectiveLength + 1);
        }
        return serverInfo;
    } else {
        throw   std::runtime_error("Error: Webserv: ");
    }
}
