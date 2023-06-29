#include <algorithm>
#include <stdexcept>
#include "server_conf.hpp"

//Constructors
ServerConf::ServerConf()
: serverBlock_() {}

ServerConf::ServerConf(const ServerConf &cpy)
: serverBlock_(cpy.serverBlock_) {}

ServerConf::ServerConf(const ServerBlock &block)
: serverBlock_(block) {}

ServerConf &ServerConf::operator=(const ServerConf &cpy) {
    ServerConf tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

//Iterators
ServerConf::LocationIterator ServerConf::locationBegin() {
    return serverBlock_.locationBlock.begin();
}

ServerConf::LocationIterator ServerConf::locationEnd() {
    return serverBlock_.locationBlock.end();
}

ServerConf::LocationConstIterator ServerConf::locationConstBegin() const {
    return serverBlock_.locationBlock.begin();
}

ServerConf::LocationConstIterator ServerConf::locationConstEnd() const {
    return serverBlock_.locationBlock.end();
}

//Peek
ServerConf::LocationIterator ServerConf::findLocation(const std::string &path) {
    LocationIterator it = locationBegin();
    while (it != locationEnd() && it->path != path) {
        it++;
    }
    return it;
}

ServerConf::LocationConstIterator ServerConf::findLocation(const std::string &path) const {
    LocationConstIterator it = locationConstBegin();
    while (it != locationConstEnd() && it->path != path) {
        it++;
    }
    return it;
}

//Server Block Modifiers
void ServerConf::setPort(uint_t port) {
    if (port > 65535) {
        throw   std::out_of_range("Invalid port");
    }
    serverBlock_.port = port;
}

void ServerConf::setHost(const std::string &host, ushort_t af) {
    if (af == AF_INET) {
        if (!inet_pton(AF_INET, host.c_str(), &serverBlock_.ipv4Host.s_addr))
            throw   std::out_of_range("Invalid host");
    } else if (af == AF_INET6) {
        if (!inet_pton(AF_INET6, host.c_str(), &serverBlock_.ipv6Host.s6_addr))
            throw   std::out_of_range("Invalid host");
    }
}

void ServerConf::addServName(const std::string &name) {
    StrVector::iterator it = std::find(serverBlock_.servNames.begin(), serverBlock_.servNames.end(), name);
    if (it == serverBlock_.servNames.end()) {
        serverBlock_.servNames.push_back(name);
    }
}

void ServerConf::addErrorPage(uint_t code, const std::string &path) {
    UshortVecMap::iterator it = serverBlock_.defErrorPage.find(code);
    if (it != serverBlock_.defErrorPage.end()) {
        if (std::find(it->second.begin(), it->second.end(), path) == it->second.end()) {
            it->second.push_back(path);
        }
    } else {
        serverBlock_.defErrorPage.insert(std::make_pair(code, StrVector(1, path)));
    }
}

void ServerConf::setMaxBytes(ulong_t bytes) {
    serverBlock_.maxBytes = bytes;
}

void ServerConf::eraseServName(const std::string &name) {
    StrVector::iterator it = std::find(serverBlock_.servNames.begin(), serverBlock_.servNames.end(), name);
    if (it != serverBlock_.servNames.end()) {
        serverBlock_.servNames.erase(it);
    }
}

void ServerConf::eraseServName(const StrVector::iterator &begin, const StrVector::iterator &end) {
    serverBlock_.servNames.erase(begin, end);
}

void ServerConf::eraseErrorPage(uint_t code, const std::string &name) {
    UshortVecMap::iterator it = serverBlock_.defErrorPage.find(code);
    if (it != serverBlock_.defErrorPage.end()) {
        StrVector::iterator pos = std::find(it->second.begin(), it->second.end(), name);
        if (pos != it->second.end()) {
            it->second.erase(pos);
        }
    }
}

void ServerConf::eraseErrorPage(uint_t code, const StrVector::iterator &begin, const StrVector::iterator &end) {
    UshortVecMap::iterator it = serverBlock_.defErrorPage.find(code);
    it->second.erase(begin, end);
}

//Location Block Modifiers
ServerConf::LocationIterator ServerConf::addLocation(const LocationBlock &block) {
    serverBlock_.locationBlock.push_back(block);
    return --locationEnd();
}

ServerConf::LocationIterator ServerConf::setLocationPath(const std::string &name, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd() || name.empty()) {
        LocationBlock newBlock = {};
        newBlock.path = name;
        newBlock.rootDir = name;
        newBlock.uploadDir = name;
        addLocation(newBlock);
        return LocationIterator(--locationEnd());
    }

    it->path = name;
    return it;
}

ServerConf::LocationIterator ServerConf::flipPermissions(uint_t method, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it != locationEnd()) {
        switch (method) {
            case 0:
                it->allowGet = !it->allowGet;
                break ;
            case 1:
                it->allowPost = !it->allowPost;
                break ;
            case 2:
                it->allowDelete = !it->allowDelete;
                break ;
        }
        return it;
    }
    return locationEnd();
}

ServerConf::LocationIterator ServerConf::addlocationRedir(uint_t code, const std::string &path, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.rootDir = dst;
        newBlock.uploadDir = dst;
        newBlock.lo_redirs.insert(std::make_pair(code, StrVector(1, path)));
        addLocation(newBlock);
        return LocationIterator(--locationEnd());
    }

    UshortVecMap::iterator mapIt = it->lo_redirs.find(code);
    if (mapIt == it->lo_redirs.end()) {
        it->lo_redirs.insert(std::make_pair(code, StrVector(1, path)));
    } else {
        mapIt->second.push_back(path);
    }
    return it;
}

ServerConf::LocationIterator ServerConf::setRootDir(const std::string &name, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.rootDir = name;
        newBlock.uploadDir = dst;
        addLocation(newBlock);
        return LocationIterator(--locationEnd());
    }

    it->rootDir = name;
    return it;
}

ServerConf::LocationIterator ServerConf::setAutoIndex(bool value, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.rootDir = dst;
        newBlock.uploadDir = dst;
        newBlock.autoIndex = value;
        addLocation(newBlock);
        return LocationIterator(--locationEnd());
    }

    it->autoIndex = value;
    return it;
}

ServerConf::LocationIterator ServerConf::addLocationIndex(const std::string &index, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.rootDir = dst;
        newBlock.uploadDir = dst;
        newBlock.lo_indexes.push_back(index);
        addLocation(newBlock);
        return LocationIterator(--locationEnd());
    }

    if (std::find(it->lo_indexes.begin(), it->lo_indexes.end(), index) == it->lo_indexes.end()) {
        it->lo_indexes.push_back(index);
    }
    return it;
}

ServerConf::LocationIterator ServerConf::setUploadDir(const std::string &path, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.rootDir = dst;
        newBlock.uploadDir = path;
        addLocation(newBlock);
        return LocationIterator(--locationEnd());
    }

    it->uploadDir = path;
    return it;
}

void ServerConf::eraseLocation(const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it != locationEnd()) {
        serverBlock_.locationBlock.erase(it);
    }
}

void ServerConf::eraseLocation(const LocationIterator &begin, const LocationIterator &end) {
    serverBlock_.locationBlock.erase(begin, end);
}

void ServerConf::clearLocations() {
    serverBlock_.locationBlock.clear();
}

void ServerConf::eraseRedir(uint_t code, const std::string &name, const std::string &dst) {
    if (!dst.empty()) {
        LocationIterator it = findLocation(dst);
        if (it != locationEnd()) {
            UshortVecMap::iterator redirIt = it->lo_redirs.find(code);
            if (redirIt != it->lo_redirs.end()) {
                StrVector::iterator pos = std::find(redirIt->second.begin(), redirIt->second.end(), name);
                if (pos != redirIt->second.end()) {
                    redirIt->second.erase(pos);
                }
            }
        }
    } else {
        UshortVecMap::iterator it = serverBlock_.se_redirs.find(code);
        if (it != serverBlock_.se_redirs.end()) {
            StrVector::iterator pos = std::find(it->second.begin(), it->second.end(), name);
            if (pos != it->second.end()) {
                it->second.erase(pos);
            }
        }
    }
}

void ServerConf::eraseIndex(const std::string &name, const std::string &dst) {
    if (!dst.empty()) {
        LocationIterator it = findLocation(dst);
        if (it != locationEnd()) {
            StrVector::iterator strIt = std::find(it->lo_indexes.begin(), it->lo_indexes.end(), name);
            if (strIt != it->lo_indexes.end()) {
                it->lo_indexes.erase(strIt);
            }
        }
    } else {
        StrVector::iterator it = std::find(serverBlock_.se_indexes.begin(), serverBlock_.se_indexes.end(), name);
        serverBlock_.se_indexes.erase(it);
    }
}


//Getters
const ServerConf::ServerBlock &ServerConf::server() const {
    return serverBlock_;
}

//Destructor
ServerConf::~ServerConf() {}
