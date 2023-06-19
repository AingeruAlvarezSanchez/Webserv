#include <map>
#include <string>
#include <sstream>
#include "server_info.hpp" //TODO maybe vector of pairs

typedef void(*serverFunction)(std::string const&, ServerInfo::s_serverData &);
typedef std::map< std::string, serverFunction >   serverRuleMap;
typedef typename std::map< std::string, serverFunction >::iterator   serverRuleMapIt;

#include <iostream>
static void    set_listen_rule(std::string const& line, ServerInfo::s_serverData & directive) {
std::cout << "listen>" << line;
}

static void    set_server_name_rule(std::string const& line, ServerInfo::s_serverData & directive) {
    size_t ruleStart = line.find_first_of(':');
    size_t ruleEnd = line.find_first_of(';');
    std::string ruleContent = line.substr(ruleStart + 1, ((ruleEnd - ruleStart) - 1));

    std::istringstream  content(ruleContent);

    std::string serverName;
    while (std::getline(content, serverName, ' ')) {
        if (serverName.find_first_of(";/?:@&=+$,") == std::string::npos) {
            if (!serverName.empty()) {
                directive.serverNames.push_back(serverName);
            }
        } else {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
        }
    }
}

static void    set_error_page_rule(std::string const& line, ServerInfo::s_serverData & directive) {
std::cout << "error_page>" << line;
}

static void    set_client_max_body_size_rule(std::string const& line, ServerInfo::s_serverData & directive) {
std::cout << "client_max_body_size>" << line;
}

void save_server_conf(ServerInfo::s_serverData & serverDirective,  std::string const& line) {
    serverRuleMap ruleMap;

    ruleMap["listen:"] = &set_listen_rule;
    ruleMap["server_name:"] = &set_server_name_rule;
    ruleMap["error_page:"] = &set_error_page_rule;
    ruleMap["client_max_body_size:"] = &set_client_max_body_size_rule;

    for (serverRuleMapIt it = ruleMap.begin(); it != ruleMap.end(); it++) {
        if (line.find(it->first) != std::string::npos
            && line.find(it->first) == line.find_first_not_of(' ')) {
            it->second(line, serverDirective);
            return ;
        }
    }
    errno = 134;
    throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
}

