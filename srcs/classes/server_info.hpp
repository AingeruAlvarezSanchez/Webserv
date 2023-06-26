#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP
#include <vector>
#include <map>
#include <fstream>
#include <cstring>

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
    typedef std::vector< std::pair< unsigned short, std::vector<std::string> > > ErrorPagesType;
    typedef std::vector< std::pair< unsigned short, std::vector<std::string> > >::iterator ErrorPagesTypeIt;
    typedef std::vector< std::pair< unsigned short, std::vector<std::string> > >::const_iterator ErrorPagesTypeConstIt;

    struct ServerBlock {
        unsigned short              serverPort;
        std::vector< std::string >  allowedHosts;
        std::vector< std::string >  serverNames;
        ErrorPagesType              errorPageRoute;
        unsigned int                maxBodyBytes;
        ConfiguredLocationsType     serverLocations;
    };

private:
    std::vector< ServerBlock >  serverDirectives_;
    std::string                 configFileName_;
    std::ifstream               configFileStream_;

    //Configuration file operations
    void eraseLineComments(std::string& line) const;
    ServerBlock fetchServerBlock(const std::string& fileContent) const;
    void fetchDirective(ServerBlock& serverBlock, const std::string& lineContent) const;
    bool isDuplicateData(const std::string& content, char c) const;
    bool isValidBlock(const std::string& block) const;
    void fillDirectiveValue(ServerBlock& serverBlock, const std::string& value, const std::string& name) const;

public:
    //Constructors
    ServerInfo();
    explicit ServerInfo(const std::string& file);
    ServerInfo(const ServerInfo& original);

    //Getters
    unsigned short getServerPort(int serverNb) const;
    std::vector< std::string >  getServerHosts(int serverNb) const;
    std::vector< std::string > getServerNames(int serverNb) const;
    unsigned int getServerMaxBytes(int serverNb) const;
    ErrorPagesTypeConstIt getServerErrorPageRoutes(int serverNb, unsigned short errorNb) const;

    //File operations
    std::string fetchStreamContent();
    void readFileConfig();

    //Operator overloads
    ServerInfo& operator=(const ServerInfo& cpy);

    //Custom error classes
    class BadSyntax : public std::exception {
    private:
        const char* _error;
    public:
        explicit BadSyntax(const std::string& msg) : _error(strdup(msg.c_str())) {}
        const char* what() const throw() {
            return _error;
        }
    };

    //Destructor
    ~ServerInfo();
};

#endif //SERVER_INFO_HPP
