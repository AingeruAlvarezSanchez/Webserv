#include "server_conf.hpp"

ServerConf::ServerConf() {}

ServerConf::ServerConf(std::vector< confValuesData > const& socketsConfigs)
    : _socketsConfData(socketsConfigs) {}

ServerConf::ServerConf(ServerConf const& cpy)
    : _socketsConfData(cpy._socketsConfData) {}

ServerConf::~ServerConf() {}

ServerConf& ServerConf::operator=(ServerConf const& cpy) {
    ServerConf tmp(cpy);

    this->_socketsConfData.swap(tmp._socketsConfData);
    return *this;
}

int ServerConf::createSocketConf(__unused const std::ifstream &file) {
    return 0;
}

bool ServerConf::serverJSONCreation() {
    return 0;
}

std::vector< ServerConf::confValuesData >::const_iterator ServerConf::getSocketConfData(int socketFd) const {
    for (std::vector< confValuesData >::const_iterator it = this->_socketsConfData.begin(); it != this->_socketsConfData.end(); it++) {
        if (it->socketFd == socketFd) {
            return it;
        }
    }
    return this->_socketsConfData.end();
}

