#include "server_conf.hpp"

ServerConf::ServerConf() {}

ServerConf::ServerConf(std::vector< socketParams > const& socketsConfigs)
    : _socketsConfData(socketsConfigs) {}

ServerConf::ServerConf(ServerConf const& cpy)
    : _socketsConfData(cpy._socketsConfData) {}

ServerConf::~ServerConf() {}

ServerConf& ServerConf::operator=(ServerConf const& cpy) {
    ServerConf tmp(cpy);

    this->_socketsConfData.swap(tmp._socketsConfData);
    return *this;
}

ServerConf::SocketParamsIterator ServerConf::getSocketConfData(int socketFd) const {
    for (SocketParamsIterator it = this->_socketsConfData.begin(); it != this->_socketsConfData.end(); it++) {
        if (it->socketFd == socketFd) {
            return it;
        }
    }
    return this->_socketsConfData.end();
}

bool ServerConf::invalidSocket(ServerConf::SocketParamsIterator const& it) const {
    return it == this->_socketsConfData.end();
}

