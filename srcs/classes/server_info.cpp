#include <sstream>
#include <cstdlib>
#include <utility>
#include "server_info.hpp"

//Constructors
ServerInfo::ServerInfo()
: serverDirectives_(), configFileName_(), configFileStream_() {}

ServerInfo::ServerInfo(const std::string& file)
: configFileName_(file), configFileStream_(file.c_str()) {
    readFileConfig();
}

ServerInfo::ServerInfo(const ServerInfo& original)
: serverDirectives_(original.serverDirectives_), configFileName_(original.configFileName_), configFileStream_(original.configFileName_.c_str()) {}

//Getters
unsigned short ServerInfo::getServerPort(int serverNb) const {
    return serverDirectives_[serverNb].serverPort;
}

std::vector<std::string> ServerInfo::getServerHosts(int serverNb) const {
    return serverDirectives_[serverNb].allowedHosts;
}

std::vector<std::string> ServerInfo::getServerNames(int serverNb) const {
    return serverDirectives_[serverNb].serverNames;
}

unsigned int ServerInfo::getServerMaxBytes(int serverNb) const {
    return serverDirectives_[serverNb].maxBodyBytes;
}

std::vector<std::string> ServerInfo::getServerErrorPageRoutes(int serverNb, unsigned short errorNb) const {
    //TODO it wont let me??
    for (ErrorPagesTypeConstIt it = serverDirectives_[serverNb].errorPageRoute.begin();
        it != serverDirectives_[serverNb].errorPageRoute.end(); it++) {
        if (errorNb == it->first) {
            return it->second;
        }
    }

    //TODO when not found
}

//Configuration file operations
void ServerInfo::eraseLineComments(std::string& line) const {
    size_t commentStart = line.find('#');
    if (commentStart != std::string::npos) {
        line.erase(commentStart, line.length() - commentStart);
    }
}

bool ServerInfo::isDuplicateData(const std::string& content, char c) const {
    if (content.find(c) != content.rfind(c)) {
        return true;
    }
    return false;
}

bool ServerInfo::isValidBlock(const std::string& block) const {
    std::string preStartContent = block.substr(6, block.find('{') - 6);

    if (preStartContent.find_first_not_of(" \t\n") != std::string::npos
        || block.find("listen:") != block.rfind("listen:")
        || block.find("listen") == std::string::npos
        || block.find("server_name:") == std::string::npos) {
        return false;
    }
    return true;
}

#include <iostream>
void ServerInfo::fillDirectiveValue(ServerBlock& serverBlock, const std::string& value, const std::string& name) const {
    if (value.find("->") == std::string::npos) {
        if (name == "listen:") {
            serverBlock.serverPort = strtol(value.c_str(), NULL, 10);
            serverBlock.allowedHosts.push_back("0.0.0.0");
        } else if (name == "server_name:") {
            serverBlock.serverNames.push_back(value);
        } else if (name == "client_max_body_size:") {
            if (value.find("mb")) {
                serverBlock.maxBodyBytes = strtol(value.c_str(), NULL, 10) * 1000000;
            } else if (value.find("mb")) {
                serverBlock.maxBodyBytes = strtol(value.c_str(), NULL, 10) * 1000;
            } else {
                serverBlock.maxBodyBytes = strtol(value.c_str(), NULL, 10);
            }
        } else {
            errno = 134;
            throw BadSyntax("Webserv: Invalid line on configuration file: " + name + value);
        }
    } else {
        std::string firstValue = value.substr(0, value.find("->"));
        std::string secondValue = value.substr(value.find("->") + 2, value.length() - value.find("->") + 2);
        if (name == "listen:") {
            serverBlock.serverPort = strtol(firstValue.c_str(), NULL, 10);
            serverBlock.allowedHosts.push_back(secondValue);
        } else if (name == "error_page:") {
            ErrorPagesTypeIt it = serverBlock.errorPageRoute.begin();
            while (it != serverBlock.errorPageRoute.end()) {
                if (it->first == strtol(firstValue.c_str(), NULL, 10)) {
                    it->second.push_back(secondValue);
                    break ;
                }
                it++;
            }
            if (it == serverBlock.errorPageRoute.end()) {
                std::pair< unsigned short, std::vector<std::string> > errorPageRoutePair;
                std::vector< std::string > newErrorPageRoutes;
                unsigned short newErrorCode = static_cast<unsigned short>(strtol(firstValue.c_str(), NULL, 10));
                newErrorPageRoutes.push_back(secondValue);

                errorPageRoutePair = std::make_pair(newErrorCode, newErrorPageRoutes);
                serverBlock.errorPageRoute.push_back(errorPageRoutePair);
            }
        } else {
            errno = 134;
            throw BadSyntax("Webserv: Invalid line on configuration file: " + name + value);
        }
    }
    //TODO
}

void ServerInfo::fetchDirective(ServerBlock& serverBlock, const std::string& lineContent) const {
    std::cout << "---------- Start of directive ---------\n";
    if (!isDuplicateData(lineContent, ';') && !isDuplicateData(lineContent, ':')
        && lineContent.find(';') != std::string::npos && lineContent.find(':') != std::string::npos) {

        size_t directiveStart = lineContent.find(':') + 1;
        size_t directiveEnd = lineContent.find(';');
        std::stringstream directiveContent(lineContent.substr(directiveStart, directiveEnd - directiveStart));

        std::string value;
        while (std::getline(directiveContent, value, ' ')) {
            if (!value.empty()) {
                size_t directiveNameStart = lineContent.find_first_not_of(" \t");
                std::string directiveName = lineContent.substr(directiveNameStart, directiveStart - directiveNameStart);

                fillDirectiveValue(serverBlock, value, directiveName);
            }
        }
    } else {
        errno = 134;
        throw BadSyntax("Webserv: Invalid line on configuration file: " + lineContent);
    }
    std::cout << "------------ End of directive -----------\n";
}

ServerInfo::ServerBlock ServerInfo::fetchServerBlock(const std::string &serverBlock) const {
    ServerBlock serverBlockInfo = {}; //TODO
    if (isValidBlock(serverBlock)) {
        size_t blockContentStart = serverBlock.find('{') + 1;
        std::string blockContent = serverBlock.substr(blockContentStart, serverBlock.length() - blockContentStart - 1);

        while (!blockContent.empty()) {
            std::string lineContent(blockContent.substr(0, blockContent.find('\n')));
            std::string locationBlock;

            eraseLineComments(lineContent);
            if (lineContent.empty() || lineContent.find_first_not_of(" \t") == std::string::npos) {
                blockContent.erase(0, blockContent.find('\n') + 1);
                continue;
            } else if (lineContent.find(';') != std::string::npos) {
                fetchDirective(serverBlockInfo, lineContent);
                blockContent.erase(0, blockContent.find('\n') + 1);
            } else if (lineContent.find("location") != std::string::npos) {
                locationBlock = blockContent.substr(0, blockContent.find(']') + 1);
                //TODO fetchLocationBlock(serverBlockInfo, lineContent); which calls fetchDirective
                blockContent.erase(0, locationBlock.length());
            } else {
                errno = 134;
                throw BadSyntax("Webserv: Invalid line on configuration file: " + lineContent);
            }
        }
    } else {
        errno = 134;
        throw BadSyntax("Webserv: Invalid server block on configuration file: " + serverBlock);
    }
    return serverBlockInfo;
}

//File operations
std::string ServerInfo::fetchStreamContent() {
    std::string lineContent;
    std::string fileContent;

    while (std::getline(configFileStream_, lineContent)) {
        size_t firstCharacter = lineContent.find_first_not_of(' ');
        if (!lineContent.empty() && lineContent[firstCharacter] != '\n') {
            fileContent += lineContent += "\n";
        }
    }
    return fileContent;
}

void ServerInfo::readFileConfig() {
    std::string fileContent(fetchStreamContent());

    while (!fileContent.empty()) {
        std::string lineContent(fileContent.substr(0, fileContent.find('\n')));
        std::string serverBlock;

        eraseLineComments(lineContent);
        if (lineContent.empty() || lineContent.find_first_not_of(" \t") == std::string::npos) {
            fileContent.erase(0, fileContent.find('\n') + 1);
            continue ;
        } else if (lineContent.find("server") != std::string::npos) {
            serverBlock = fileContent.substr(0, fileContent.find('}') + 1);
            serverDirectives_.push_back(fetchServerBlock(serverBlock));
        } else {
            errno = 134;
            throw BadSyntax("Webserv: Invalid line on configuration file: " + lineContent);
        }
        fileContent.erase(0, serverBlock.length());
    }
}

//Operator overloads
ServerInfo &ServerInfo::operator=(const ServerInfo& cpy) {
    ServerInfo  tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

ServerInfo::~ServerInfo() {}
