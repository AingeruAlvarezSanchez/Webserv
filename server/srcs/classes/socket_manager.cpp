#include <cstring>
#include <iostream>
#include <climits>
#include "socket_manager.hpp"

//Constructors
SocketManager::SocketManager() : sockets_() {}

SocketManager::SocketManager(const SocketManager &cpy) : sockets_(cpy.sockets_) {}

SocketManager::SocketManager(ServerConf &conf, int domain, int type) {
    int fd = socket(domain, type, 0);
    if (fd != -1) {
        sockets_.insert(std::make_pair(fd, conf));
    }
}

SocketManager::SocketManager(ConfIter first, ConfIter last, int type) {
    while (first != last) {
        int fd;
        if (first->server().ipv4Addr.sin_addr.s_addr != UINT_MAX) {
            fd = socket(first->server().ipv4Addr.sin_family, type, 0);
        } else {
            fd = socket(first->server().ipv6Addr.sin6_family, type, 0);
        }
        if (fd != -1) {
            sockets_.insert(std::make_pair(fd, first->server()));
        }
        first++;
    }
}

SocketManager &SocketManager::operator=(const SocketManager &cpy) {
    SocketManager tmp(cpy);

    std::swap(tmp, *this);
    return *this;
}

//Iterators
SocketManager::SockIter SocketManager::sockBegin() {
    return sockets_.begin();
}

SocketManager::SockIter SocketManager::sockEnd() {
    return sockets_.end();
}

//Socket map modifiers
SocketManager::SockIter SocketManager::addSocket(const ServerConf &server, int domain, int type) {
    int fd = socket(domain, type, 0);
    if (fd != -1) {
        sockets_.insert(std::make_pair(fd, server));
    }
    return --sockEnd();
}

SocketManager::SockIter SocketManager::swapSockConf(int sockFd, const ServerConf &conf) {
    SockIter it = sockets_.find(sockFd);
    if (it != sockets_.end()) {
        it->second = conf;
    }

    return it;
}

//Socket operation functions
std::vector<int> SocketManager::listenOnSock() {
    std::vector<int>    validFds;

    for (SockIter it = sockets_.begin(); it != sockets_.end(); it++) {
        if (it->second.server().ipv4Addr.sin_addr.s_addr < UINT_MAX) {
            struct sockaddr_in addr = it->second.server().ipv4Addr;
            bind(it->first, (struct sockaddr *)&addr, sizeof(addr));
            if (listen(it->first, 0) != -1) {
                validFds.push_back(it->first);
            }
        } else {
            struct sockaddr_in6 addr = it->second.server().ipv6Addr;
            bind(it->first, (struct sockaddr *) &addr, sizeof(addr));
            if (listen(it->first, 0) != -1) {
                validFds.push_back(it->first);
            }
        }
    }
    return validFds;
}

int SocketManager::listenOnSock(SockIter it) {
    if (it->second.server().ipv4Addr.sin_addr.s_addr < UINT_MAX) {
        struct sockaddr_in addr = it->second.server().ipv4Addr;
        if (bind(it->first, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            std::cerr << "Socket manager: bind: " << strerror(errno) << "\n";
            return -1;
        }
        if (listen(it->first, 0) == -1) {
            std::cerr << "Socket manager: listen: " << strerror(errno) << "\n";
            return -1;
        }
    } else {
        struct sockaddr_in6 addr = it->second.server().ipv6Addr;
        if (bind(it->first, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
            std::cerr << "Socket manager: bind: " << strerror(errno) << "\n";
            return -1;
        }
        if (listen(it->first, 0) == -1) {
            std::cerr << "Socket manager: listen: " << strerror(errno) << "\n";
            return -1;
        }
    }
    return it->first;
}

std::vector<int> SocketManager::listenOnSock(SockIter first, SockIter last) {
    std::vector<int>    validFds;

    while (first != last) {
        if (first->second.server().ipv4Addr.sin_addr.s_addr < UINT_MAX) {
            struct sockaddr_in addr = first->second.server().ipv4Addr;
            bind(first->first, (struct sockaddr *)&addr, sizeof(addr));
            if (listen(first->first, 0) != -1) {
                validFds.push_back(first->first);
            }
        } else {
            struct sockaddr_in6 addr = first->second.server().ipv6Addr;
            bind(first->first, (struct sockaddr *) &addr, sizeof(addr));
            if (listen(first->first, 0) != -1) {
                validFds.push_back(first->first);
            }
        }
        first++;
    }
    return validFds;
}

SocketManager::~SocketManager() {}
