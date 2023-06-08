#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP
#include <vector>
#include "server_conf.hpp"

class ServerHandler {
public:
    //Constructors
    ServerHandler();
    ServerHandler(ServerHandler const& cpy);

    //Destructor
    ~ServerHandler();

    //Operator overloads
    ServerHandler& operator=(ServerHandler const& cpy);

    //Request handler functions
    bool getRequestHandler(std::string const& getReceivedContent) const; //TODO bool es temporal, quiero devolver un map que jsonize
    bool postRequestHandler(std::string const& postReceivedContent) const; //TODO
    bool deleteRequestHandler(std::string const& deleteReceivedContent) const; //TODO

private:
    std::vector<int>    _portSockets;
    ServerConf          _ServerConfig;
};

#endif //SERVER_HANDLER_HPP
