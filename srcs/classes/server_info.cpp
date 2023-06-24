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
bool ServerInfo::isValidConfigLine(const std::string& line) const {
    size_t firstCharacter = line.find_first_not_of(' ');

    if (line[firstCharacter] != '#' && firstCharacter != line.find("server")) {
        return false;
    }
    return true;
}

#include <iostream> //TODO
ServerInfo::ServerBlock ServerInfo::fetchServerBlock(const std::string &fileContent) const {
    std::string serverBlockContent = fileContent.substr(0, fileContent.find_first_of('}') + 1);
    std::cout << "serverBlock>" << serverBlockContent << "\n";

    ServerBlock serverBlock = {}; //TODO
    return serverBlock;
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

#include <cstdlib>
std::vector< ServerInfo::ServerBlock > ServerInfo::readFileConfig() {
    std::string fileContent(fetchStreamContent());

    std::vector< ServerBlock > serverConfigurations; //TODO return
    while (!fileContent.empty()) {
        size_t newLine = fileContent.find_first_of('\n') + 1;
        std::string lineContent(fileContent.substr(0, newLine));

        if (lineContent.find("server") != std::string::npos) {
            serverConfigurations.push_back(fetchServerBlock(fileContent));

            std::string tmp = fileContent.substr(fileContent.find_first_of('}') + 1, fileContent.length());
            //erasefunct(fileContent);
        } else if (isValidConfigLine(lineContent)) {
            fileContent.erase(0, newLine);
        } else {
            errno = 134;
            throw BadSyntax("Webserv: Invalid line on configuration file: " + lineContent);
        }
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
