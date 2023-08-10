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
    serverBlock_.ipv4Addr.sin_port = htons(port);
    serverBlock_.ipv6Addr.sin6_port = htonl(port);
}

void ServerConf::setHost(const std::string &host, ushort_t domain) {
    if (domain == AF_INET) {
        if (!inet_pton(AF_INET, host.c_str(), &serverBlock_.ipv4Addr.sin_addr.s_addr)) {
            throw   std::out_of_range("Invalid host");
        }
        serverBlock_.ipv4Addr.sin_family = AF_INET;
    } else if (domain == AF_INET6) {
        if (!inet_pton(AF_INET6, host.c_str(), &serverBlock_.ipv6Addr.sin6_addr)) {
            throw   std::out_of_range("Invalid host");
        }
        serverBlock_.ipv6Addr.sin6_family = AF_INET6;
        serverBlock_.ipv4Addr.sin_addr.s_addr = UINT_MAX;
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

void ServerConf::setMaxBytes(uint_t bytes) {
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
        newBlock.uploadDir = name;
        newBlock.allowGet = true;
        addLocation(newBlock);
        return --locationEnd();
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

ServerConf::LocationIterator ServerConf::addLocationRedir(uint_t code, const std::string &path, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.uploadDir = dst;
        newBlock.redirect = std::make_pair(code, path);
        newBlock.allowGet = true;
        addLocation(newBlock);
        return --locationEnd();
    }

    it->redirect = std::make_pair(code, path);
    return it;
}

ServerConf::LocationIterator ServerConf::addLocationAltFile(ServerConf::uint_t code, const std::string &path, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.uploadDir = dst;
        newBlock.try_files.first = code;
        newBlock.try_files.second.push_back(path);
        newBlock.allowGet = true;
        addLocation(newBlock);
        return --locationEnd();
    }

    if (it->try_files.second.empty()) {
        it->try_files.first = code;
        it->try_files.second.push_back(path);
    } else if (std::find(it->try_files.second.begin(), it->try_files.second.end(), path) == it->try_files.second.end()) {
        it->try_files.second.push_back(path);
    }
    return it;
}

ServerConf::LocationIterator ServerConf::setRootDir(const std::string &name, const std::string &dst) {
    if (dst.empty()) {
        serverBlock_.rootDir = name;
        return locationEnd();
    }

    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.uploadDir = dst;
        newBlock.rootDir = name;
        newBlock.allowGet = true;
        addLocation(newBlock);
        return --locationEnd();
    }

    it->rootDir = name;
    return it;
}

ServerConf::LocationIterator ServerConf::setAutoIndex(bool value, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.uploadDir = dst;
        newBlock.autoIndex = value;
        newBlock.allowGet = true;
        addLocation(newBlock);
        return --locationEnd();
    }

    it->autoIndex = value;
    return it;
}

ServerConf::LocationIterator ServerConf::addLocationIndex(const std::string &index, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.uploadDir = dst;
        newBlock.indexes.push_back(index);
        newBlock.allowGet = true;
        addLocation(newBlock);
        return --locationEnd();
    }

    if (std::find(it->indexes.begin(), it->indexes.end(), index) == it->indexes.end()) {
        it->indexes.push_back(index);
    }
    return it;
}

ServerConf::LocationIterator ServerConf::addLocationCgi(const std::string &cgi, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.uploadDir = dst;
        newBlock.cgiLangs.push_back(cgi);
        newBlock.allowGet = true;
        addLocation(newBlock);
        return --locationEnd();
    }

    if (std::find(it->cgiLangs.begin(), it->cgiLangs.end(), cgi) == it->cgiLangs.end()) {
        it->cgiLangs.push_back(cgi);
    }
    return it;
}

ServerConf::LocationIterator ServerConf::setUploadDir(const std::string &path, const std::string &dst) {
    LocationIterator it = findLocation(dst);
    if (it == locationEnd()) {
        LocationBlock newBlock = {};
        newBlock.path = dst;
        newBlock.uploadDir = path;
        newBlock.allowGet = true;
        addLocation(newBlock);
        return --locationEnd();
    }

    it->uploadDir = path;
    return it;
}

void ServerConf::clearLocations() {
    serverBlock_.locationBlock.clear();
}

//Getters
const ServerConf::ServerBlock &ServerConf::server() const {
    return serverBlock_;
}

//Destructor
ServerConf::~ServerConf() {}

//Overload for operator<<
std::ostream    &operator<<(std::ostream &os, const ServerConf &serv) {
    os << "################ START ##############\n";
    os << "Port: ";
    if (serv.server().ipv4Addr.sin_addr.s_addr != UINT_MAX) {
        os << ntohs(serv.server().ipv4Addr.sin_port) << "\n";
        os << "Host: ";
        os << inet_ntoa(serv.server().ipv4Addr.sin_addr) << "\n";
    } else {
        os << serv.server().ipv6Addr.sin6_port << "\n";
        os << "Host: ";
    }
    os << "Aliases:\n";
    for (ServerConf::StrVector::const_iterator it = serv.server().servNames.begin();
            it != serv.server().servNames.end(); it++)
    os << " -> " << *it << "\n";
    os << "General root directory: " << serv.server().rootDir << "\n";
    os << "Error redirections:\n";
    for (std::map<ServerConf::ushort_t, std::vector<std::string> >::const_iterator it = serv.server().defErrorPage.begin();
        it != serv.server().defErrorPage.end(); it++) {
        os << " -> " << it->first << "\n";
        for (ServerConf::StrVector::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            os << "     * " << *it2 << "\n";
        }
    }
    os << "Max bytes: " << serv.server().maxBytes << "\n";
    for (ServerConf::LocationConstIterator it = serv.locationConstBegin(); it != serv.locationConstEnd(); it++) {
        os << "----------\n";
        os << "Location: " << it->path << "\n";
        os << " -> Location root: " << it->rootDir << "\n";
        os << " -> Allowed methods:\n";
        os << "     * GET: " << it->allowGet << "\n";
        os << "     * POST: " << it->allowPost << "\n";
        os << "     * DELETE: " << it->allowDelete << "\n";
        os << " -> Upload Dir: " << it->uploadDir << "\n";
        os << " -> Auto Index: " << it->autoIndex << "\n";
        os << " -> Index list:\n";
        for (ServerConf::StrVector::const_iterator it2 = it->indexes.begin(); it2 != it->indexes.end(); it2++) {
            os << "     * " << *it2 << "\n";
        }
        os << " -> Cgi Languages:\n";
        for (ServerConf::StrVector::const_iterator it2 = it->cgiLangs.begin(); it2 != it->cgiLangs.end(); it2++) {
            os << "     * " << *it2 << "\n";
        }
        os << " -> Redirection:\n";
        os << "     * Code: " << it->redirect.first << "\n";
        os << "     * new URL: " << it->redirect.second << "\n";
        os << " -> Alt files (try_files):\n";
        os << "     * Code: " << it->try_files.first << "\n";
        for (std::vector<std::string>::const_iterator it2 = it->try_files.second.begin();
             it2 != it->try_files.second.end(); it2++) {
            os << "     * " << *it2 << "\n";
        }
        os << "----------\n";
    }
    os << "################ END ##############\n";
    return os;
}
