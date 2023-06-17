#ifndef WEBSERV_H
#define WEBSERV_H

#include "../srcs/classes/server_info.hpp" //TODO change from Makefile for only including server_info.hpp

#define ESYNTAX 134 //Syntax error, used for all syntax cases.

ServerInfo  config_file_parsing(char const *fileName);
ServerInfo::s_serverData  get_directive_conf(std::string & serverDirective);

//Set rules
void    set_limit_except_rule(std::string const& line, ServerInfo::s_location & location);
void    set_return_rule(std::string const& line, ServerInfo::s_location & location);
void    set_root_rule(std::string const& line, ServerInfo::s_location & location);
void    set_try_files_rule(std::string const& line, ServerInfo::s_location & location);
void    set_auto_index_rule(std::string const& line, ServerInfo::s_location & location);
void    set_index_rule(std::string const& line, ServerInfo::s_location & location);
void    set_cgi_pass_rule(std::string const& line, ServerInfo::s_location & location);
void    set_upload_rule(std::string const& line, ServerInfo::s_location & location);

#endif //WEBSERV_H
