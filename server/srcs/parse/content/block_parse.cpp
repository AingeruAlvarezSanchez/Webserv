#include "webserv.h"

std::string  fetch_block(const std::string &content, size_t start, size_t end) {
    std::string block = content.substr(start, end - start);
    return block;
}

std::string get_path(const std::string &block) {
    std::string result;
    std::string line;

    if (block.find("location") + 8 == block.find(':')){
        line = block.substr(block.find(':') + 1, block.find('[') - block.find(':') - 1);
    } else {
        return "";
    }
    line.erase(0, line.find_first_not_of(" \t"));

    size_t delim = line.find_first_of(" \t");
    std::string tmp;
    if (delim != std::string::npos) {
        tmp = line.substr(line.find_first_of(" \t"), line.length() - line.find_first_of(" \t"));
    }

    if (tmp.empty() || tmp.find_first_not_of(" \t") == std::string::npos) {
        result = line.substr(0, delim);
    }

    if (result.empty()) {
        return result;
    }

    if (result.at(0) == '/') {
        result.erase(0, 1);
    }
    if (result.length() == 0) {
        result += '/';
    } else if (result.at(result.length() - 1) != '/') {
        result += '/';
    }

    return result;
}

int parse_block(std::string &block, ServerConf &serverConf, const std::string &path, const std::string &section) {
    while (block.find_first_not_of(" \t\n") != std::string::npos) {
        size_t endl = block.find('\n');
        std::string line = block.substr(0, endl + 1);
        size_t directiveEnd = line.find(';');

        if (directiveEnd != std::string::npos) {
            std::string content = fetch_directive(line);
            if (parse_directive(content, serverConf, path) == -1) {
                return -1;
            }
            block.erase(0, directiveEnd + 1);
        } else if (!section.empty() && line.find(section) != std::string::npos) {
            std::string newPath = get_path(block);
            if (newPath.empty()) {
                return -1;
            }
            serverConf.setLocationPath(newPath, "");
            std::string newBlock = fetch_block(block, block.find('[') + 1, block.find(']'));
            size_t size = block_size(block, ']');
            if (parse_block(newBlock, serverConf, newPath) == -1) {
                return -1;
            }
            block.erase(0, size + 1);
        } else if (is_valid_line(line)) {
            block.erase(0, endl + 1);
        } else {
        }
    }
    return 0;
}
