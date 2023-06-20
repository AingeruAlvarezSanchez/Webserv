#include <map>
#include <string>
#include <sstream>
#include "server_info.hpp" //TODO maybe vector of pairs

typedef void(*serverFunction)(std::string const&, ServerInfo::s_serverData &);
typedef std::map< std::string, serverFunction >   serverRuleMap;
typedef typename std::map< std::string, serverFunction >::iterator   serverRuleMapIt;

static std::string const get_rule_content(std::string const& line) {
    size_t ruleStart = line.find_first_of(':');
    size_t ruleEnd = line.find_first_of(';');
    std::string ruleContent = line.substr(ruleStart + 1, ((ruleEnd - ruleStart) - 1));

    if (ruleContent.find_first_not_of(' ') != std::string::npos) {
        ruleContent.erase(0, ruleContent.find_first_not_of(' '));
    } else {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
    }
    return ruleContent;
}

static void    set_listen_rule(std::string const& line, ServerInfo::s_serverData & directive) {
    std::string ruleContent = get_rule_content(line);

    std::string port;
    if (ruleContent.find("->") == std::string::npos) {
        //TODO function for when there is no ->
        port = ruleContent.substr(ruleContent.find_first_not_of(' '), ruleContent.length() - ruleContent.find_first_not_of(' '));
        for (std::string::iterator it = port.begin(); it != port.end(); it++) {
            if (!std::isdigit(*it)) {
                errno = 134;
                throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
            }
        }

        if (std::atoi(port.c_str()) > 0 && std::atoi(port.c_str()) <= 65535) {
            directive.serverPort = std::atoi(port.c_str());
        } else {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
        }
        directive.allowedHosts.push_back("0.0.0.0");
    } else {
        //TODO function for when there is ->
        std::string hosts = ruleContent.substr(ruleContent.find("->") + 2, (ruleContent.length() - ruleContent.find("->")) + 2);
        std::string buffer;
        if (hosts.find_first_not_of(' ') != std::string::npos) {
            std::istringstream content(hosts);

            while (std::getline(content, buffer, ' ')) {
                if (!buffer.empty()) {
                    //TODO this could go on another function, multiple if for while is obscene
                    for (std::string::iterator it = buffer.begin(); it != buffer.end(); it++) {
                        if (*it == '.') {
                            if (!std::isdigit(*(it + 1)) && (it + 1) != buffer.end()) {
                                errno = 134;
                                throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
                            }
                        } else if (!std::isdigit(*it)) {
                            errno = 134;
                            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
                        }
                    }
                    directive.allowedHosts.push_back(buffer);
                }
            }
        } else {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
        }
    }
}

static void    set_server_name_rule(std::string const& line, ServerInfo::s_serverData & directive) {
    std::string ruleContent = get_rule_content(line);
    std::string serverName;

    std::istringstream  content(ruleContent);
    static bool alreadyExists;
    while (std::getline(content, serverName, ' ')) {
        if (serverName.find_first_of(";/?:@&=+$,") == std::string::npos) {
            if (!serverName.empty()) {
                if (alreadyExists) {
                    directive.serverNames.clear();
                    alreadyExists = false;
                }
                directive.serverNames.push_back(serverName);
            }
        } else {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
        }
    }
    alreadyExists = true;
}

#include <iostream>
static void    set_error_page_rule(std::string const& line, ServerInfo::s_serverData & directive) {
    std::string ruleContent = get_rule_content(line);

    std::cout << "error_page>" << ruleContent << "\n";
    //TODO we need a default error 404 page
}

static void    set_client_max_body_size_rule(std::string const& line, ServerInfo::s_serverData & directive) {
    std::string ruleContent = get_rule_content(line);

    if (ruleContent.find_first_of(' ') != std::string::npos) {
        std::string postContent = ruleContent.substr(ruleContent.find_first_of(' '), ruleContent.length() - ruleContent.find_first_of(' '));
        if (postContent.find_first_not_of(' ') != std::string::npos) {
            errno = 134;
            throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
        }
        ruleContent.erase(ruleContent.find_first_of(' '), ruleContent.length() - ruleContent.find_first_of(' '));
    }

    unsigned long size;
    for (std::string::iterator it = ruleContent.begin(); it != ruleContent.end(); it++) {
        if (!std::isdigit(*it)) {
            std::string number = ruleContent.substr(0, (it - ruleContent.begin()));
            size = std::atoi(number.c_str());
            break ;
        }
    }

    if (ruleContent.find("kb") != std::string::npos) {
        directive.maxBodyBytes = (size * 1000);
    } else if (ruleContent.find("mb") != std::string::npos) {
        directive.maxBodyBytes = (size * 1000000);
    } else if (ruleContent.find("gb") != std::string::npos) {
        directive.maxBodyBytes = (size * 1000000000);
    } else if (ruleContent.find('b') != std::string::npos) {
        directive.maxBodyBytes = size;
    } else {
        errno = 134;
        throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
    }
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

