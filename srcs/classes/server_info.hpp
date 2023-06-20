#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP
#include <vector>
#include <string>

#include <iostream> //TODO
class ServerInfo {
public:
    typedef struct {
        bool                        getRequestPermission;
        bool                        postRequestPermission;
        bool                        deleteRequestPermission;
        std::vector< std::string >  pageFileNames;          //TODO check later
        std::string                 root;
        bool                        autoIndex;
        std::vector< std::string >  indexFileNames;
        std::vector< std::string >  cgiLanguages;
        std::string                 uploadsRoute;
    }   s_location;

    typedef std::pair < std::string, s_location >                                locationDirective;
    typedef std::vector< std::pair< std::string, s_location > >                  configuredLocations;
    typedef std::vector< std::pair< std::string, s_location > >::const_iterator  configuredLocationsIterator;
    typedef std::vector< std::pair< unsigned short, std::vector<std::string> > > errorPageRoutes;

    typedef struct {
        unsigned short              serverPort;
        std::vector< std::string >  allowedHosts;
        std::vector< std::string >  serverNames;
        errorPageRoutes             errorPageRoute; //TODO this must be a vector of pairs< error_num, route >
        unsigned long               maxBodyBytes;
        configuredLocations         serverLocations;
    }   s_serverData;

    typedef std::vector< std::pair< std::vector< int >, s_serverData > >                  serverInfo;
    typedef std::vector< std::pair< std::vector< int >, s_serverData > >::const_iterator  serverInfoIterator;
    typedef std::pair< std::vector< int >, s_serverData >                                 serverInfoPair;

private:
    serverInfo  _serverInfo;

public:
    //Constructors
    ServerInfo();
    ServerInfo(ServerInfo const& original);

    //Getters
    //TODO getters for all the members of the different structs

    //Setters
    void    setServerData(s_serverData const& value);

    //Operator overloads
    ServerInfo& operator=(ServerInfo const& cpy);

    //Custom error classes
    class BadSyntax : public std::exception {
    private:
        char const* _error;
    public:
        explicit BadSyntax(char const* msg) : _error(msg) {}
        char const* what() const throw() {
            return this->_error;
        }
    };

    //Destructor
    ~ServerInfo();
};

#endif //SERVER_INFO_HPP
