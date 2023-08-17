#include <fstream>
#include <fcntl.h>
#include "webserv.h"

int check_arguments(int argc, const std::string &file) {
    if (argc < 2) {
        errno = EINVAL;
        return -1;
    } else if (argc > 2) {
        errno = E2BIG;
        return -1;
    }

    if (file.find_last_of('.') == std::string::npos || file.substr(file.find_last_of('.')) != ".conf") {
        errno = EINVAL;
        return -1;
    } else if (open(file.c_str(), O_RDONLY) == -1) {
        errno = EINVAL;
        return -1;
    }
    return 0;
}

std::string config_file_content(const std::string &file) {
    std::ifstream stream(file.c_str());
    std::string line;
    std::string content;
    while (std::getline(stream, line)) {
        content += line += '\n';
    }
    return content;
}

int config_file_parser(std::string &content, std::vector<ServerConf> &serverConf) {
    while (!content.empty()) {
        size_t endl = content.find('\n');
        std::string line = content.substr(0, endl + 1);
        if (line.find("server") != std::string::npos) {
            size_t size = block_size(content, '}');
            std::string block = fetch_block(content, content.find('{') + 1, content.find('}'));
            serverConf.push_back(ServerConf());
            if (parse_block(block, serverConf.back(), "", "location") == -1) {
                return -1;
            }
            content.erase(0, size + 1);
        } else if (is_valid_line(line)) {
            content.erase(0, endl + 1);
        } else {
            return -1;
        }
    }
    return 0;
}
