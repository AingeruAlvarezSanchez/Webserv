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

#include <iostream> //TODO
//TODO maybe change location of typedef
template <typename T>
struct ruleAliases {
    typedef std::map< std::string, void(*)(std::string const&, T &) >   ruleMap;
    typedef typename std::map< std::string, void(*)(std::string const&, T &) >::iterator ruleMapIterator;
};

template <typename T>
static void save_directive_conf(T & directive, std::string const& line) {
    typename ruleAliases<T>::ruleMap    locationRuleMap;

    locationRuleMap["listen:"] = &ServerInfo::set_listen_rule;
    locationRuleMap["server_name:"] = &ServerInfo::set_server_name_rule;
    locationRuleMap["error_page:"] = &ServerInfo::set_error_page_rule;
    locationRuleMap["client_max_body_size:"] = &ServerInfo::set_client_max_body_size_rule;

    locationRuleMap["limit_except:"] = &ServerInfo::set_limit_except_rule;
    locationRuleMap["return:"] = &ServerInfo::set_return_rule;
    locationRuleMap["root:"] = &ServerInfo::set_root_rule;
    locationRuleMap["try_files:"] = &ServerInfo::set_try_files_rule;
    locationRuleMap["auto_index:"] = &ServerInfo::set_auto_index_rule;
    locationRuleMap["index:"] = &ServerInfo::set_index_rule;
    locationRuleMap["cgi_pass:"] = &ServerInfo::set_cgi_pass_rule;
    locationRuleMap["upload:"] = &ServerInfo::set_upload_rule;

    for (typename ruleAliases<T>::ruleMapIterator it = locationRuleMap.begin(); it != locationRuleMap.end(); it++) {
        if (line.find(it->first) != std::string::npos
            && line.find(it->first) == line.find_first_not_of(' ')) {
            it->second(line, directive);
            return ;
        }
    }
    errno = 134;
    throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join
}

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

//TODO if something is after the location ] it must throw an exception, it doesnt atm
static ServerInfo::locationDirective    get_location_conf(std::string & locationDirective) {
    std::string locationPath = get_location_path(locationDirective);
    locationDirective = erase_directive_delimiters(locationDirective, '[');

    ServerInfo::s_location location = {}; //TODO
    std::cout << "------------------ location -------------------------\n";
    while (!locationDirective.empty() && locationDirective.find_first_not_of(" \n") != std::string::npos) {
        std::string line = locationDirective.substr(0, locationDirective.find_first_of('\n') + 1);
        size_t ruleSemicolon = line.find_first_of(';');

        is_valid_comment_line(line, ruleSemicolon);
        save_directive_conf(location, line);

        locationDirective.erase(0, line.length() + 1);
    }
    std::cout << "------------------ location end -------------------------\n";
    return std::make_pair(locationPath, location); //TODO
}

static void get_general_rule_conf(ServerInfo::s_serverData &data, std::string const& line, size_t const& ruleSemicolon) {
    is_valid_comment_line(line, ruleSemicolon);
    save_directive_conf(data, line);
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
                size_t locationStart = serverDirective.find("location:");
                size_t locationEnd = serverDirective.find_first_of(']');
                std::string locationDirective = serverDirective.substr(locationStart,((locationEnd + 1) - locationStart));

                serverDirective.erase(locationStart, locationDirective.length() + 1);
                data.serverLocations.push_back(get_location_conf(locationDirective));
            } else {
                get_general_rule_conf(data, line, ruleSemicolon);
            }
        }
        serverDirective.erase(0, line.length());
    }
    return data;
}