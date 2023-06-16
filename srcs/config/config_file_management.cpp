#include <fstream>
#include <cstring>
#include "../classes/server_info.hpp"

static std::string get_server_directive(std::string configContent) {
    size_t  firstNonSpace   = configContent.find_first_not_of(' ');
    size_t  serverDirectiveStart    = configContent.find("server");

    if (configContent[firstNonSpace] == configContent[serverDirectiveStart]) {
        std::string serverDirectiveContent("server");
        configContent.erase(0, std::strlen("server"));

        size_t  apertureBrace   = configContent.find_first_of('{');
        firstNonSpace   = configContent.find_first_not_of(' ');

        if (configContent[firstNonSpace] == configContent[apertureBrace]) {
            serverDirectiveContent.append(configContent, 0, (configContent.find_first_of('}') + 1));
        } else {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
        }

        if (serverDirectiveContent.find_first_of('{') != serverDirectiveContent.find_last_of('{')) {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
        }
        return  serverDirectiveContent;
    } else {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
}

#include <iostream> //TODO
static bool get_location_conf(std::string const& line) {
    std::cout << "location: " << line;
    return false;
}

static bool get_rule_conf(std::string const& line, size_t const& ruleSemicolon) {
    std::string commentLine = line.substr(ruleSemicolon + 1, (line.length() - ruleSemicolon - 2));

    if (commentLine.find_first_not_of(' ') != std::string::npos
        && commentLine.find_first_not_of(' ') != commentLine.find_first_of('#')) {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
    std::cout << "line: " << line;
    return false;
}

bool    rule_content_wrapper(std::string const& line, size_t const& ruleSemicolon) { //TODO put on new directory
    if (line.find("location:") != std::string::npos) {
        get_location_conf(line);
    } else {
        get_rule_conf(line, ruleSemicolon);
    }
    return false;
}

static ServerInfo::serverData   get_directive_conf(std::string &serverDirective) {
    ServerInfo::serverData data = {}; //TODO
    size_t  directiveStart = serverDirective.find_first_of('{');
    size_t  ruleNewLine = serverDirective.find_first_of('\n');
    //TODO function
    std::string line = serverDirective.substr(directiveStart + 1, (ruleNewLine - directiveStart - 1));
    size_t  lineComment = line.find_first_of('#');

    if (line.find_first_not_of(' ') != std::string::npos && line.find_first_not_of(' ') != lineComment) {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
    serverDirective.erase(0, (ruleNewLine + 1));
    serverDirective.erase(serverDirective.find_last_of('}'), 1);
    //TODO function

    size_t ruleSemicolon;
    while (!serverDirective.empty()) {
        line = serverDirective.substr(0, serverDirective.find_first_of('\n') + 1);
        ruleSemicolon = line.find_first_of(';');

        if (ruleSemicolon == std::string::npos
            && line.find("location:") == std::string::npos) {
            std::cout << "Error on line: " << line;
        } else {
            rule_content_wrapper(line, ruleSemicolon);
        }
        serverDirective.erase(0, line.length());
    }
    return data;
}

ServerInfo  config_file_parsing(char const *fileName) {
    std::ifstream   file(fileName);

    if (file.is_open()) {
        std::string buffer;
        std::string configContent;

        while (file.good()) { //TODO function
            std::getline(file, buffer);

            size_t firstNonSpace = buffer.find_first_not_of(' ');
            if ((buffer.find_first_not_of(' ') != std::string::npos && buffer[firstNonSpace] == '#')
                || buffer.empty()) {
                continue;
            }
            configContent += buffer += '\n';
        }

        ServerInfo  serverInfo;
        while (!configContent.empty()) { //TODO function
            std::string serverDirective = get_server_directive(configContent);
            size_t      serverDirectiveLength = serverDirective.length();

            serverInfo.setServerInfo(get_directive_conf(serverDirective));
            configContent.erase(0, serverDirectiveLength + 1);
        }
        return serverInfo;
    } else {
        throw std::runtime_error("Error: Webserv: ");
    }
}
