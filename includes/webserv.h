#ifndef WEBSERV_H
#define WEBSERV_H

#include "../srcs/classes/server_info.hpp" //TODO change from Makefile for only including server_info.hpp

#define ESYNTAX 134 //Syntax error, used for all syntax cases.

ServerInfo  config_file_parsing(std::string const& fileName);
ServerInfo::s_serverData  get_directive_conf(std::string & serverDirective);

#endif //WEBSERV_H
