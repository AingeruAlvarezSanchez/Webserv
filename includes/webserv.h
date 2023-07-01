#ifndef WEBSERV_WEBSERV_H
#define WEBSERV_WEBSERV_H

#include "socket_manager.hpp"

//Config file
std::string config_file_content(const std::string &file);
int config_file_parser(std::string &content, std::vector<ServerConf> &serverConf);

//Block content
std::string  fetch_block(const std::string &content, size_t start, size_t end);
int parse_block(std::string &block, ServerConf &serverConf, const std::string &path, const std::string &section = "");

//Directive content
std::string fetch_directive(const std::string &line);
int parse_directive(const std::string &content, ServerConf &serverConf, const std::string &path);

//Set rules
int  port_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  host_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  server_name_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  error_page_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  max_body_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  limit_except_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  return_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  root_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  try_files_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  auto_index_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  index_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  cgi_pass_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);
int  upload_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code);

//Utils
bool    is_valid_line(const std::string &line);
size_t  block_size(const std::string &content, char delim);

#endif //WEBSERV_WEBSERV_H
