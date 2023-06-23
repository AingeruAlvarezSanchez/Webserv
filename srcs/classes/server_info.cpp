#include <sstream>
#include "server_info.hpp"

//Constructors
ServerInfo::ServerInfo()
: serverDirectives_(), configFileStream_(), configFileName_() {}

ServerInfo::ServerInfo(const std::string& file)
: configFileStream_(file), configFileName_(file) {
    serverDirectives_ = readFileConfig(this->configFileStream_);
}

ServerInfo::ServerInfo(const ServerInfo& original)
: serverDirectives_(original.serverDirectives_), configFileStream_(original.configFileName_), configFileName_(original.configFileName_) {}

//Configuration file syntax
#include <iostream> //TODO
bool ServerInfo::validConfigLine(const std::string& line) {
    size_t firstCharacter = line.find_first_not_of(' ');

    if (line[firstCharacter] != '#' && firstCharacter != line.find("server")) {
        return false;
    }
    return true;
}

//File operations
std::string ServerInfo::fetchStreamContent(std::ifstream& fileStream) {
    std::string lineContent;
    std::string fileContent;

    while (std::getline(fileStream, lineContent)) {
        size_t firstCharacter = lineContent.find_first_not_of(' ');
        if (!lineContent.empty() && lineContent[firstCharacter] != '\n') {
            fileContent += lineContent += "\n";
        }
    }
    return fileContent;
}

std::vector< ServerInfo::ServerData > ServerInfo::readFileConfig(std::ifstream& fileStream) {
    std::string fileContent(fetchStreamContent(fileStream));

    std::vector< ServerData > serverDirectives; //TODO return
    while (!fileContent.empty()) {
        size_t newLine = fileContent.find_first_of('\n') + 1;
        std::string lineContent(fileContent.substr(0, newLine));

        if (lineContent.find("server") != std::string::npos) {
            std::cout << "server>" << lineContent << "\n";
        } else if (!validConfigLine(lineContent)) {
            errno = 134;
            throw BadSyntax("Webserv: Invalid line on configuration file: " + lineContent);
        }
        fileContent.erase(0, newLine);
    }
    std::cout << "fileContent2>" << fileContent;
    return serverDirectives;
}

//Operator overloads
ServerInfo &ServerInfo::operator=(const ServerInfo& cpy) {
    ServerInfo  tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

ServerInfo::~ServerInfo() {}
