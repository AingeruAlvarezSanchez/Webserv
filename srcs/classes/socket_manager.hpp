#ifndef WEBSERV_SOCKET_MANAGER_HPP
#define WEBSERV_SOCKET_MANAGER_HPP

#include "server_conf.hpp"

class SocketManager {
public:
    //Non-class dependant definitions
    typedef std::map<int, ServerConf>::iterator  SockIter;
    typedef std::vector<ServerConf>::iterator    ConfIter;

private:
    std::map<int, ServerConf>   sockets_;

public:
    //Constructors
    SocketManager();
    SocketManager(const SocketManager &cpy);
    SocketManager(ServerConf &conf, int domain, int type);
    SocketManager(ConfIter first, ConfIter last, int type);
    SocketManager &operator=(const SocketManager &cpy);

    //Iterators
    SockIter    sockBegin();
    SockIter    sockEnd();

    //Socket map modifiers
    SockIter    addSocket(const ServerConf &server, int domain, int type);
    SockIter    swapSockConf(int sockFd, const ServerConf &conf, int domain, int type);

    //Socket operation functions
    int listenOnSock(SockIter it);
    std::vector<int> listenOnSock();

    //Destructor
    ~SocketManager();
};


#endif //WEBSERV_SOCKET_MANAGER_HPP
