#include "webserv.h"
#include "../../classes/server_conf.hpp" //TODO just for clion
#include <sstream>

#include <iostream> //TODO
static std::string get_complete_value(const std::string &content) {
    std::string line;
    std::string value;
    std::istringstream is(content, std::ios::in);
    while (std::getline(is, line, ' ')) {
        if (!line.empty()) {
            value += line;
        }
    }
    return value;
}

int  port_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    std::string content = get_complete_value(value);
    char *end;
    long num = strtol(content.c_str(), &end, 10);
    if (*end != '\0') {
        return -1;
    }
    serverConf.setPort(num);
    return 0;
}

int  host_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    std::string content = get_complete_value(value);
    if (content.empty()) {
        content = "0.0.0.0";
    }
    serverConf.setHost(content, AF_INET);
    return 0;
}

int  server_name_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    std::string line;
    std::istringstream is(value, std::ios::in);
    while (std::getline(is, line, ' ')) {
        if (!line.empty()) {
            serverConf.addServName(line);
        }
    }
    return 0;
}

int  error_page_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}

int  max_body_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    std::string content = get_complete_value(value);
    char *end;
    long num = strtol(value.c_str(), &end, 10);

    std::string endStr(end);
    for (std::string::iterator it = endStr.begin(); it != endStr.end(); it++) {
        *it = std::tolower(*it);
    }

    if (endStr.find("mb") != std::string::npos && endStr.find("mb") + 2 >= endStr.size()) {
        serverConf.setMaxBytes(num * 1000000);
        return 0;
    } else if (endStr.find("kb") != std::string::npos && endStr.find("kb") + 2 >= endStr.size()) {
        serverConf.setMaxBytes(num * 1000);
        return 0;
    }  else if (endStr.find("b") != std::string::npos && endStr.find("b") + 2 >= endStr.size() || *end == '\0') {
        serverConf.setMaxBytes(num);
        return 0;
    }
    return -1;
}

int  limit_except_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}

int  return_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}

int  root_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}

int  try_files_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}

int  auto_index_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}

int  index_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}

int  cgi_pass_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}

int  upload_set(ServerConf &serverConf, const std::string &value, const std::string &code) {
    return 0;
}
