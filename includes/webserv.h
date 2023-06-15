#ifndef WEBSERV_H
#define WEBSERV_H

#include "../srcs/classes/server_info.hpp" //TODO change from Makefile for only including server_info.hpp

#define ESYNTAX 134 //Syntax error, used for all syntax cases.

ServerInfo  config_file_parsing(char const *fileName);

#endif //WEBSERV_H
