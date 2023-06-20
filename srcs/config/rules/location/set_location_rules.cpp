#include <map>
#include <string>
#include "server_info.hpp" //TODO maybe vector of pairs

typedef void(*locationFunction)(std::string const&, ServerInfo::s_location &);
typedef std::map< std::string, locationFunction >   locationRuleMap;
typedef typename std::map< std::string, locationFunction >::iterator   locationRuleMapIt;

#include <iostream> //TODO
static void    set_limit_except_rule(std::string const& line, ServerInfo::s_location & directive) {
    //std::cout << "limit_except>" << line;
}

static void    set_return_rule(std::string const& line, ServerInfo::s_location & directive) {
    //std::cout << "return>" << line;
}

static void    set_root_rule(std::string const& line, ServerInfo::s_location & directive) {
    //std::cout << "root>" << line;
}

static void    set_try_files_rule(std::string const& line, ServerInfo::s_location & directive) {
    //std::cout << "try_files>" << line;
}

static void    set_auto_index_rule(std::string const& line, ServerInfo::s_location & directive) {
    //std::cout << "auto_index>" << line;
}

static void    set_index_rule(std::string const& line, ServerInfo::s_location & directive) {
    //std::cout << "index>" << line;
}

static void    set_cgi_pass_rule(std::string const& line, ServerInfo::s_location & directive) {
    //std::cout << "cgi_pass>" << line;
}

static void    set_upload_rule(std::string const& line, ServerInfo::s_location & directive) {
    //std::cout << "upload>" << line;
}

void save_location_conf(ServerInfo::s_location & locationDirective, std::string const& line) {
    locationRuleMap ruleMap;

    ruleMap["limit_except:"] = &set_limit_except_rule;
    ruleMap["return:"] = &set_return_rule;
    ruleMap["root:"] = &set_root_rule;
    ruleMap["try_files:"] = &set_try_files_rule;
    ruleMap["auto_index:"] = &set_auto_index_rule;
    ruleMap["index:"] = &set_index_rule;
    ruleMap["cgi_pass:"] = &set_cgi_pass_rule;
    ruleMap["upload:"] = &set_upload_rule;

    for (locationRuleMapIt it = ruleMap.begin(); it != ruleMap.end(); it++) {
        if (line.find(it->first) != std::string::npos
            && line.find(it->first) == line.find_first_not_of(' ')) {
            it->second(line, locationDirective);
            return ;
        }
    }
    errno = 134;
    throw   ServerInfo::BadSyntax("Error: Webserv: Bad syntax"); //TODO maybe as a detail i can give the exact point of error with join*/
}
