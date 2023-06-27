#include "server_conf.hpp"

//Constructors
ServerConf::ServerConf() : serverBlock_() {}

ServerConf::ServerConf(const ServerConf &cpy) : serverBlock_(cpy.serverBlock_) {}

ServerConf &ServerConf::operator=(const ServerConf &cpy) {
    ServerConf tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

ServerConf::ServerConf(const ServerConf::ServerBlock &block) : serverBlock_(block) {}

//Iterators
ServerConf::locationConstIte ServerConf::locationBegin() const {
    return serverBlock_.locationBlock.begin();
}

ServerConf::locationConstIte ServerConf::locationEnd() const {
    return serverBlock_.locationBlock.end();
}

//Peek
ServerConf::locationConstIte ServerConf::findLocation(const std::string &route) const {
    locationConstIte it = serverBlock_.locationBlock.begin();
    while (it != serverBlock_.locationBlock.end() && it->routeName != route) {
        it++;
    }
    return it;
}

//Modifiers
void ServerConf::setPort(unsigned int port) {
    if (port > 65535) {
        throw   std::out_of_range("Invalid port");
    }
    serverBlock_.port = port;
}

void ServerConf::setHost(const std::string &host) {
    if (inet_addr(host.c_str()) == -1)
        throw   std::out_of_range("Invalid host");
    serverBlock_.host = inet_addr(host.c_str());
}

void ServerConf::setMaxBytes(unsigned long bytes) {
    serverBlock_.maxBytes = bytes;
}

void ServerConf::addServName(const std::string &name) {
    strVectorIte it = std::find(serverBlock_.servNames.begin(), serverBlock_.servNames.end(), name);
    if (it == serverBlock_.servNames.end()) {
        serverBlock_.servNames.push_back(name);
    }
}

void ServerConf::addErrorPage(unsigned int code, const std::string &route) {
    codeRouteIte it = serverBlock_.defErrorPage.begin();

    while (it != serverBlock_.defErrorPage.end()) {
        if (it->first == code) {
            if (std::find(it->second.begin(), it->second.end(), route) == it->second.end()) {
                it->second.push_back(route);
            }
            return ;
        }
        it++;
    }

    std::vector<std::string>    redir(1, route);
    serverBlock_.defErrorPage.push_back(std::make_pair(code, redir));
}

void ServerConf::addLocation(const ServerConf::LocationBlock &block) {
    serverBlock_.locationBlock.push_back(block);
}

ServerConf::locationConstIte ServerConf::addLocationRedir(const std::string &blockRoute, const std::string &route,
                                                          unsigned int code) {
    locationConstIte it = findLocation(blockRoute);
    std::vector<std::string>    redir(1, route);

    if (it == serverBlock_.locationBlock.end()) {
        LocationBlock newBlock = {};
        newBlock.redirRoute.push_back(std::make_pair(code, redir));
        addLocation(newBlock);
        return locationConstIte(--serverBlock_.locationBlock.end());
    }
    LocationBlock& location = serverBlock_.locationBlock[it - serverBlock_.locationBlock.begin()];
    location.redirRoute.push_back(std::make_pair(code, redir));
    return it;
}

ServerConf::locationConstIte ServerConf::addLocationIndex(const std::string &blockRoute, const std::string &index) {
    locationConstIte it = findLocation(blockRoute);

    if (it == serverBlock_.locationBlock.end()) {
        LocationBlock newBlock = {};
        newBlock.indexArray.push_back(index);
        addLocation(newBlock);
        return locationConstIte(--serverBlock_.locationBlock.end());
    }
    serverBlock_.locationBlock[it - serverBlock_.locationBlock.begin()].indexArray.push_back(index);
    return it;
}

//Getters
const ServerConf::ServerBlock &ServerConf::serverBlock() const {
    return serverBlock_;
}

ServerConf::locationConstIte ServerConf::locationBlock(const std::string &route) const {
    return findLocation(route);
}

//Destructor
ServerConf::~ServerConf() {}
