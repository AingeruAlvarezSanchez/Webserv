#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP
#include <vector>

class ServerInfo {
public:
    typedef struct {
        int                         requestPermissions[3];
        std::vector< std::string >  pageFileNames; //TODO check later
        std::string                 root;
        bool                        autoIndex;
        std::vector< std::string >  indexFileNames;
        std::vector< std::string >  cgiLanguages;
        std::string                 uploadsRoute;
    }   _location;

    typedef std::vector< std::pair< std::string, _location > >  t_configuredLocations;
    typedef struct {
        std::vector< int >          socketFd;
        std::vector< std::string >  serverNames;
        std::string                 errorPageRoute;
        unsigned int                maxBodySize;        //TODO conversions
        t_configuredLocations       serverLocations;
    }   _server;
public:
    //Constructors
    ServerInfo();
    ServerInfo(ServerInfo const& cpy);

    //Operator overloads
    ServerInfo& operator=(ServerInfo const& cpy);

    //Destructor
    ~ServerInfo();
};

#endif //SERVER_INFO_HPP
