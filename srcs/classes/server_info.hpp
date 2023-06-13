#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP
#include <vector>
#include <string>

class ServerInfo {
public:
    typedef struct {
        bool                        requestPermissions[3];  //TODO care, deep copy
        std::vector< std::string >  pageFileNames;          //TODO check later
        std::string                 root;
        bool                        autoIndex;
        std::vector< std::string >  indexFileNames;
        std::vector< std::string >  cgiLanguages;
        std::string                 uploadsRoute;
    }   _location;

    typedef std::vector< std::pair< std::string, _location > >                  t_configuredLocations;
    typedef std::vector< std::pair< std::string, _location > >::const_iterator  configuredLocationsIterator;

    typedef struct {
        std::vector< std::string >  serverNames;
        std::string                 errorPageRoute;
        unsigned int                maxBodySize;        //TODO conversions
        t_configuredLocations       serverLocations;
    }   serverData;

    typedef std::vector< std::pair< std::vector< int >, serverData > >                  serverInfo;
    typedef std::vector< std::pair< std::vector< int >, serverData > >::const_iterator  serverInfoIterator;

private:
    serverInfo  _serverInfo;
public:
    //Constructors
    ServerInfo();
    ServerInfo(ServerInfo const& original);

    //Operator overloads
    ServerInfo& operator=(ServerInfo const& cpy);

    //Destructor
    ~ServerInfo();
};

#endif //SERVER_INFO_HPP