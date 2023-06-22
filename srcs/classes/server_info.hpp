#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP
#include <vector>
#include <string>
#include <fstream>

class ServerInfo {
public:
    struct Location {
        bool                        getRequestPermission;
        bool                        postRequestPermission;
        bool                        deleteRequestPermission;
        //std::vector< std::string >  pageFileNames;          //TODO check later
        std::string                 root;
        bool                        autoIndex;
        std::vector< std::string >  indexFileNames;
        std::vector< std::string >  cgiLanguages;
        std::string                 uploadsRoute;
    };

    typedef std::pair < std::string, Location >                 LocationDirectiveType;
    typedef std::vector< std::pair< std::string, Location > >   ConfiguredLocationsType;

    struct ServerData {
        unsigned short              serverPort;
        std::vector< std::string >  allowedHosts;
        std::vector< std::string >  serverNames;
        //ErrorPageRoutesType         errorPageRoute; //TODO    typedef std::vector< std::pair< unsigned short, std::vector<std::string> > > ErrorPageRoutesType; //TODO change name??
        unsigned long               maxBodyBytes;
        ConfiguredLocationsType     serverLocations;
    };

private:
    std::vector< ServerData >   serverDirectives_;
    std::string                 configFileName_;
    std::ifstream               configFileStream_;

public:
    //Constructors
    ServerInfo();
    ServerInfo(const std::string& file);
    ServerInfo(const ServerInfo& original);

    //File operations
    std::vector< ServerInfo::ServerData > readFileConfig(std::ifstream& file);

    //Getters


    //Operator overloads
    ServerInfo& operator=(const ServerInfo& cpy);

    //Custom error classes
    class BadSyntax : public std::exception {
    private:
        const char* _error;
    public:
        explicit BadSyntax(const char* msg) : _error(msg) {}
        const char* what() const throw() {
            return this->_error;
        }
    };

    //Destructor
    ~ServerInfo();
};

#endif //SERVER_INFO_HPP
