#include "server_info.hpp"

ServerInfo::ServerInfo() {} //TODO

ServerInfo::ServerInfo(const ServerInfo &original)
: _serverInfo(original._serverInfo) {} //TODO in theory just by doing this I already do deep copy, but check later.

ServerInfo &ServerInfo::operator=(const ServerInfo &cpy) {
    ServerInfo  tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

ServerInfo::~ServerInfo() {}
