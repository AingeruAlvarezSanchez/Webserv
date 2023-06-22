#include "server_info.hpp"

//Constructors
ServerInfo::ServerInfo()
: serverDirectives_(), configFileStream_(), configFileName_() {}

ServerInfo::ServerInfo(const std::string& file)
: configFileStream_(file), configFileName_(file) {
    this->serverDirectives_ = readFileConfig(this->configFileStream_);
}

ServerInfo::ServerInfo(const ServerInfo& original)
: serverDirectives_(original.serverDirectives_), configFileStream_(original.configFileName_), configFileName_(original.configFileName_) {}

//File operations
#include <iostream> //TODO
std::vector< ServerInfo::ServerData > ServerInfo::readFileConfig(std::ifstream& file) {
    std::vector< ServerData > serverDirectives; //TODO return
    std::string line;

    while (std::getline(file, line)) {
        std::cout << "line>" << line << "\n";
    }
    return serverDirectives;
}

//Operator overloads
ServerInfo &ServerInfo::operator=(const ServerInfo& cpy) {
    ServerInfo  tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

ServerInfo::~ServerInfo() {}
