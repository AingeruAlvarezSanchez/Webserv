#ifndef WEBSERV_H
#define WEBSERV_H

#include "server_info.hpp"

#define ESYNTAX 134 //Syntax error, used for all syntax cases.

ServerInfo  config_file_parsing(std::string const& fileName);
ServerInfo::s_serverData  get_directive_conf(std::string & serverDirective);
ServerInfo::locationDirective   get_location_conf(std::string & locationDirective);
void get_general_rule_conf(ServerInfo::s_serverData &data, std::string const& line, size_t const& ruleSemicolon);

#endif //WEBSERV_H
