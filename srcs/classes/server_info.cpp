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

#include <iostream>
void ServerInfo::fetchDirective(ServerInfo::ServerBlock &serverBlock, const std::string& lineContent) const {
    //TODO check for more errors, for the moment will be a simple parse for testing
    if (isDuplicateData(lineContent, ';') || isDuplicateData(lineContent, ':')) {
        errno = 134;
        throw BadSyntax("Webserv: Invalid line on configuration file: " + lineContent);
    }
    std::string directive = lineContent.substr(lineContent.find_first_not_of(" \t"), lineContent.find(';'));
    std::cout << "line>" << lineContent << "<end\n";
}

ServerInfo::ServerBlock ServerInfo::fetchServerBlock(const std::string &serverBlock) const {
    //TODO errors between "server" and '{'
    size_t blockContentStart = serverBlock.find('{') + 1;
    std::string blockContent = serverBlock.substr(blockContentStart, serverBlock.length() - blockContentStart - 1);

    std::cout << "----------- Start of server block ----------\n";
    ServerBlock serverBlockInfo = {}; //TODO
    while (!blockContent.empty()) {
        std::string lineContent(blockContent.substr(0, blockContent.find('\n')));
        std::string locationBlock;

        eraseLineComments(lineContent);
        if (lineContent.empty() || lineContent.find_first_not_of(" \t") == std::string::npos) {
            blockContent.erase(0, blockContent.find('\n') + 1);
            continue ;
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
    std::cout << "------------ End of server block -----------\n";
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
