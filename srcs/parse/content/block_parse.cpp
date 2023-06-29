#include "webserv.h"

std::string  fetch_block(const std::string &content, size_t start, size_t end) {
    std::string block = content.substr(start, end - start);
    return block;
}

int parse_block(std::string &block, ServerConf &serverConf, const std::string &section) {
    while (block.find_first_not_of(" \t\n") != std::string::npos) {
        size_t endl = block.find('\n');
        std::string line = block.substr(0, endl + 1);
        size_t directiveEnd = line.find(';');

        if (directiveEnd != std::string::npos) {
            std::string content = fetch_directive(line);
            if (parse_directive(content, serverConf) == -1) {
                return -1;
            }
            block.erase(0, directiveEnd + 1);
        } else if (!section.empty() && line.find(section) != std::string::npos) {
            std::string newBlock = fetch_block(block, block.find('[') + 1, block.find(']'));
            size_t size = block_size(block, ']');
            if (parse_block(newBlock, serverConf) == -1) {
                return -1;
            }
            block.erase(0, size + 1);
        } else if (is_valid_line(line)) {
            block.erase(0, endl + 1);
        } else {
            return -1;
        }
    }
    return 0;
}
