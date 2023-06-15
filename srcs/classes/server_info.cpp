#include "server_info.hpp"

ServerInfo::ServerInfo() {} //TODO

void ServerInfo::setServerInfo(serverData const &data) {
    serverInfoPair serverDataPair = std::make_pair(std::vector< int >(), data);

    this->_serverInfo.push_back(serverDataPair);
}

ServerInfo::ServerInfo(const ServerInfo &original)
: _serverInfo(original._serverInfo) {} //TODO in theory just by doing this I already do deep copy, but check later.

ServerInfo &ServerInfo::operator=(const ServerInfo &cpy) {
    ServerInfo  tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

ServerInfo::~ServerInfo() {}
