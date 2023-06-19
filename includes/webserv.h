#ifndef WEBSERV_H
#define WEBSERV_H

#include "server_info.hpp"

#define ESYNTAX 134 //Syntax error, used for all syntax cases.

//general purpose parsers
ServerInfo  config_file_parsing(std::string const& fileName);
ServerInfo::s_serverData  parse_server_conf(std::string & serverDirective);

//server rules parsers
ServerInfo::locationDirective   get_location_conf(std::string & locationDirective);
void get_server_rule_conf(ServerInfo::s_serverData &data, std::string const& line, size_t const& ruleSemicolon);

//server rules configuration
void save_server_conf(ServerInfo::s_serverData & serverDirective, std::string const& line);
void save_location_conf(ServerInfo::s_location & locationDirective, std::string const& line);

#endif //WEBSERV_H
