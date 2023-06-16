#include <cerrno>
#include "../classes/server_info.hpp"

static std::string erase_directive_delimiters(std::string const& directive, bool const& directiveType) {
    size_t  directiveStart;
    if (directiveType == 0) {
        directiveStart = directive.find_first_of('{');
    } else {
        directiveStart = directive.find_first_of('[');
    }

    size_t  ruleNewLine = directive.find_first_of('\n');
    std::string line = directive.substr(directiveStart + 1, (ruleNewLine - directiveStart - 1));
    size_t  lineComment = line.find_first_of('#');

    if (line.find_first_not_of(' ') != std::string::npos
        && line.find_first_not_of(' ') != lineComment) {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
    std::string result = directive.substr((ruleNewLine + 1), (directive.length() - (ruleNewLine + 1) - 1));
    return result;
}

#include <iostream> //TODO
//TODO if something is before the rule name (somelocation:, somelisten:) it must throw an exception, it doesnt atm
//TODO if something is after the location ] it must throw an exception, it doesnt atm
static ServerInfo::locationDirective    get_location_conf(std::string & locationDirective) {
    size_t locationContentStart = locationDirective.find_first_of('[');
    size_t locationPathStart = locationDirective.find_first_not_of("location: ");

    std::string locationPath = locationDirective.substr(locationPathStart, (locationContentStart - locationPathStart));
    size_t locationPathEnd = locationPath.find_first_of(' ');
    locationPath.erase(locationPathEnd, locationPath.length());

    locationDirective = erase_directive_delimiters(locationDirective, 1);

    ServerInfo::s_location location = {};
    return std::make_pair(locationPath, location); //TODO
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

ServerInfo::s_serverData   get_directive_conf(std::string & serverDirective) {
    serverDirective = erase_directive_delimiters(serverDirective, 0);

    ServerInfo::s_serverData data = {}; //TODO
    if (serverDirective.find("listen:") == std::string::npos
        || serverDirective.find("server_name:") == std::string::npos) {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
    while (!serverDirective.empty() && serverDirective.find_first_not_of(" \n") != std::string::npos) {
        std::string line = serverDirective.substr(0, serverDirective.find_first_of('\n') + 1);
        size_t ruleSemicolon = line.find_first_of(';');

        if (ruleSemicolon == std::string::npos
            && line.find("location:") == std::string::npos) {
            std::cout << "Error on line: " << line;
        } else {
            if (line.find("location:") != std::string::npos) {
                size_t locationStart = serverDirective.find("location:");
                size_t locationEnd = serverDirective.find_first_of(']');
                std::string locationDirective = serverDirective.substr(locationStart,((locationEnd + 1) - locationStart));

                serverDirective.erase(locationStart, locationDirective.length() + 1);
                data.serverLocations.push_back(get_location_conf(locationDirective));
            } else {
                get_rule_conf(line, ruleSemicolon);
            }
        }
        serverDirective.erase(0, line.length());
    }
    return data;
}