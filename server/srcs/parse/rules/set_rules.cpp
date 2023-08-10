#include <sstream>
#include "webserv.h"

static std::string erase_value_delimiters(const std::string &content) {
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

static std::string format_path(const std::string &path) {
    std::string newPath = path;

    if (newPath.at(0) == '/' && newPath.length() > 1) {
        newPath.erase(0, 1);
    }
    if (newPath.at(newPath.length() - 1) != '/') {
        newPath += '/';
    }

    return newPath;
}

int  port_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string content = erase_value_delimiters(value);
    char *end;
    long num = strtol(content.c_str(), &end, 10);
    if (*end != '\0') {
        return -1;
    }
    serverConf.setPort(num);
    return 0;
}

int  host_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string content = erase_value_delimiters(value);
    if (content.empty()) {
        content = "0.0.0.0";
    }
    serverConf.setHost(content, AF_INET);
    return 0;
}

int  server_name_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string line;
    std::istringstream is(value, std::ios::in);
    while (std::getline(is, line, ' ')) {
        if (line.find('\t') != std::string::npos) {
            return -1;
        }
        if (!line.empty()) {
            serverConf.addServName(line);
        }
    }
    return 0;
}

int  error_page_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string codeCont = erase_value_delimiters(code);
    char *end;
    long num = strtol(codeCont.c_str(), &end, 10);
    if (*end != '\0') {
        return -1;
    }

    std::string line;
    std::istringstream is(value, std::ios::in);
    while (std::getline(is, line, ' ')) {
        if (line.find('\t') != std::string::npos) {
            return -1;
        }
        if (!line.empty()) {
            if (!serverConf.server().rootDir.empty()) {
                serverConf.addErrorPage(num, serverConf.server().rootDir + line);
            } else {
                serverConf.addErrorPage(num, line);
            }
        }
    }
    return 0;
}

int  max_body_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string content = erase_value_delimiters(value);
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
    }  else if (endStr.find('b') != std::string::npos && endStr.find('b') + 2 >= endStr.size() || *end == '\0') {
        serverConf.setMaxBytes(num);
        return 0;
    }
    return -1;
}

int  limit_except_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    if (value == "allow_all") {
        std::string line;
        std::istringstream is(code, std::ios::in);
        while (std::getline(is, line, ' ')) {
            if (line == "GET") {
                serverConf.flipPermissions(GET, format_path(path));
            } else if (line == "POST") {
                serverConf.flipPermissions(POST, format_path(path));
            } else if (line == "DELETE") {
                serverConf.flipPermissions(DELETE, format_path(path));
            }
        }
    } else if (value == "deny_all") {
        if (code.find("GET") == std::string::npos) {
            serverConf.flipPermissions(GET, format_path(path));
        }
        if (code.find("POST") == std::string::npos) {
            serverConf.flipPermissions(POST, format_path(path));
        }
        if (code.find("DELETE") == std::string::npos) {
            serverConf.flipPermissions(DELETE, format_path(path));
        }
    }
    return 0;
}

int  return_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string line;
    std::istringstream is(value, std::ios::in);
    while (std::getline(is, line, ' ')) {
        if (line.find('\t') != std::string::npos) {
            return -1;
        }
        if (!line.empty()) {
            serverConf.addLocationRedir(std::atoi(code.c_str()), value, format_path(path));
        }
    }
    return 0;
}

int  root_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string content = erase_value_delimiters(value);
    if (content.at(content.length() - 1) != '/') {
        content += '/';
    }

    if (path.empty()) {
        serverConf.setRootDir(content);
    } else {
        serverConf.setRootDir(content, format_path(path));
    }
    return 0;
}

int  try_files_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string line;
    std::istringstream is(value, std::ios::in);
    while (std::getline(is, line, ' ')) {
        if (line.find('\t') != std::string::npos) {
            return -1;
        }
        if (!line.empty()) {
            serverConf.addLocationAltFile(std::atoi(code.c_str()), line, format_path(path));
        }
    }
    return 0;
}

int  auto_index_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string content = erase_value_delimiters(value);
    if (content.find("on") != std::string::npos) {
        if (content.find_first_not_of(" \t") != content.find("on")) {
            return -1;
        }
        serverConf.setAutoIndex(true, format_path(path));
    } else if (content.find("off") != std::string::npos) {
        if (content.find_first_not_of(" \t") != content.find("off")) {
            return -1;
        }
        serverConf.setAutoIndex(false, format_path(path));
    } else {
        return -1;
    }
    return 0;
}

int  index_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string line;
    std::istringstream is(value, std::ios::in);
    while (std::getline(is, line, ' ')) {
        if (line.find('\t') != std::string::npos) {
            return -1;
        }
        if (!line.empty()) {
            serverConf.addLocationIndex(line, format_path(path));
        }
    }
    return 0;
}

int  cgi_pass_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string line;
    std::istringstream is(value, std::ios::in);
    while (std::getline(is, line, ' ')) {
        if (line.find('\t') != std::string::npos) {
            return -1;
        }
        if (!line.empty()) {
            serverConf.addLocationCgi(line, format_path(path));
        }
    }
    return 0;
}

int  upload_set(ServerConf &serverConf, const std::string &value, const std::string &path, const std::string &code) {
    std::string content = value.substr(value.find_first_not_of(" \t"));
    size_t end = content.find_first_of(" \t");
    if (end != std::string::npos) {
        if (!is_valid_line(content.substr(end, content.length()))
            || content.substr(end, content.length()).find('#') != std::string::npos) {
            return -1;
        }
        serverConf.setUploadDir(content.substr(0, end), format_path(path));
    } else {
        serverConf.setUploadDir(content, format_path(path));
    }
    return 0;
}
