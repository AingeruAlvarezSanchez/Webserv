#include "webserv.h"

bool    is_valid_line(const std::string &line) {
    size_t pos = line.find_first_not_of(" \t\n");
    if (pos != std::string::npos && line[pos] != '#') {
        return false;
    }
    return true;
}

size_t  block_size(const std::string &content, char delim) {
    size_t blockEnd = content.find(delim);
    std::string server = content.substr(0, blockEnd + 1);
    return server.size();
}
