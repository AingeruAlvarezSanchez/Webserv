#include <cerrno>
#include <map>
#include "webserv.h"
#include "server_info.hpp"

static std::string erase_directive_delimiters(std::string const& directive, unsigned char const& type) {
    size_t  directiveStart;
    if (type == '{') {
        directiveStart = directive.find_first_of('{');
    } else {
        directiveStart = directive.find_first_of('[');
    }

    size_t  firstNewLine = directive.find_first_of('\n');
    std::string firstLine = directive.substr(directiveStart + 1, (firstNewLine - directiveStart - 1));
    size_t  lineComment = firstLine.find_first_of('#');

    if (firstLine.find_first_not_of(' ') != std::string::npos
        && firstLine.find_first_not_of(' ') != lineComment) {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
    std::string result = directive.substr((firstNewLine + 1), (directive.length() - firstNewLine - 2));
    return result;
}

ServerInfo::s_serverData   get_directive_conf(std::string & serverDirective) {
    serverDirective = erase_directive_delimiters(serverDirective, '{');

    if (serverDirective.find("listen:") == std::string::npos
        || serverDirective.find("server_name:") == std::string::npos) {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }

    ServerInfo::s_serverData data = {}; //TODO
    while (!serverDirective.empty() && serverDirective.find_first_not_of(" \n") != std::string::npos) {
        std::string line = serverDirective.substr(0, serverDirective.find_first_of('\n') + 1);
        size_t ruleSemicolon = line.find_first_of(';');

        if (ruleSemicolon == std::string::npos
            && line.find("location:") == std::string::npos) {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
        } else {
            if (line.find("location:") != std::string::npos) {
                //TODO this could be a whole funcion
                size_t locationStart = serverDirective.find("location:");
                size_t locationEnd = serverDirective.find_first_of(']');
                std::string locationDirective = serverDirective.substr(locationEnd + 1, \
                                                (serverDirective.length() - (locationEnd + 1)));
                if (locationDirective.find_first_not_of(' ') != std::string::npos
                    && locationDirective.find_first_not_of(' ') != locationDirective.find_first_of('\n')) {
                    errno = 134;
                    throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
                }
                locationDirective = serverDirective.substr(locationStart, ((locationEnd + 1) - locationStart));

                serverDirective.erase(locationStart, locationDirective.length() + 1);
                locationDirective = erase_directive_delimiters(locationDirective, '[');
                data.serverLocations.push_back(get_location_conf(locationDirective));
            } else {
                get_general_rule_conf(data, line, ruleSemicolon);
            }
        }
        serverDirective.erase(0, line.length());
    }
    return data;
}