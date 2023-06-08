#ifndef WEBSERV_H
#define WEBSERV_H
#include <string>
#include "../srcs/classes/server_conf.hpp" //TODO want relative route, not full

#define ESYNTAX 134 //Syntax error on configuration file.

ServerConf const& conf_file_parsing(std::string const &fileName);

#endif //WEBSERV_H
