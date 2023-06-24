#include "server_info.hpp"

//Constructors
ServerInfo::ServerInfo()
: serverDirectives_(), configFileName_(), configFileStream_() {}

ServerInfo::ServerInfo(const std::string& file)
: configFileName_(file), configFileStream_(file.c_str()) {
    serverDirectives_ = readFileConfig();
}

ServerInfo::ServerInfo(const ServerInfo& original)
: serverDirectives_(original.serverDirectives_), configFileName_(original.configFileName_), configFileStream_(original.configFileName_.c_str()) {}

//Configuration file operations
void ServerInfo::eraseLineComments(std::string& line) const {
    size_t commentStart = line.find('#');
    if (commentStart != std::string::npos) {
        line.erase(commentStart, line.length() - commentStart);
    }
}

#include <iostream>
ServerInfo::ServerBlock ServerInfo::fetchServerBlock(const std::string &fileContent) const {
    std::cout << "content>" << fileContent << "<end\n";

    ServerBlock serverBlockInfo = {}; //TODO
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

std::vector< ServerInfo::ServerBlock > ServerInfo::readFileConfig() {
    std::string fileContent(fetchStreamContent());

    std::vector< ServerBlock > serverConfigurations; //TODO return
    while (!fileContent.empty()) {
        std::string lineContent(fileContent.substr(0, fileContent.find('\n')));
        std::string serverBlock;

        eraseLineComments(lineContent);
        if (lineContent.empty() || lineContent.find_first_not_of(" \t") == std::string::npos) {
            fileContent.erase(0, fileContent.find('\n') + 1);
            continue ;
        } else if (lineContent.find("server") != std::string::npos) {
            serverBlock = fileContent.substr(0, fileContent.find('}') + 1);
            serverConfigurations.push_back(fetchServerBlock(serverBlock));
        } else {
            errno = 134;
            throw BadSyntax("Webserv: Invalid line on configuration file: " + lineContent);
        }
        fileContent.erase(0, serverBlock.length());
    }
    return serverConfigurations;
}

//Operator overloads
ServerInfo &ServerInfo::operator=(const ServerInfo& cpy) {
    ServerInfo  tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

ServerInfo::~ServerInfo() {}
