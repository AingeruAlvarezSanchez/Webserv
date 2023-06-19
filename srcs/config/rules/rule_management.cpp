#include "webserv.h"
#include <map> //TODO maybe vector of pairs

static void is_valid_comment_line(std::string const& line, size_t const& ruleSemicolon) {
    std::string commentLine = line.substr(ruleSemicolon + 1, (line.length() - ruleSemicolon - 2));

    if (commentLine.find_first_not_of(' ') != std::string::npos
        && commentLine.find_first_not_of(' ') != commentLine.find_first_of('#')) {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }
}

static std::string get_location_path(std::string const& locationDirective) {
    size_t locationContentStart = locationDirective.find_first_of('[');
    size_t locationPathStart = locationDirective.find_first_not_of("location: ");
    std::string locationPath = locationDirective.substr(locationPathStart, (locationContentStart - locationPathStart));

    if (locationPath.empty()) {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
    }

    size_t locationPathEnd = locationPath.find_first_of(' ');
    locationPath.erase(locationPathEnd, locationPath.length());
    return locationPath;
}

ServerInfo::locationDirective    get_location_conf(std::string & locationDirective) {
    std::string locationPath = get_location_path(locationDirective);

    ServerInfo::s_location location = {}; //TODO
    std::cout << "------------------ location -------------------------\n";
    while (!locationDirective.empty() && locationDirective.find_first_not_of(" \n") != std::string::npos) {
        std::string line = locationDirective.substr(0, locationDirective.find_first_of('\n') + 1);
        size_t ruleSemicolon = line.find_first_of(';');

        is_valid_comment_line(line, ruleSemicolon);
        save_location_conf(location, line);

        locationDirective.erase(0, line.length() + 1);
    }
    std::cout << "------------------ location end -------------------------\n";
    return std::make_pair(locationPath, location); //TODO
}

void get_server_rule_conf(ServerInfo::s_serverData &data, std::string const& line, size_t const& ruleSemicolon) {
    is_valid_comment_line(line, ruleSemicolon);
    save_server_conf(data, line);
}
