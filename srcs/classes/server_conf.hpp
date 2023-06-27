#ifndef WEBSERV_SERVERCONF_HPP
#define WEBSERV_SERVERCONF_HPP

#include <arpa/inet.h>
#include <vector>
#include <string>

#define POST    0
#define GET     1
#define DELETE  2

class ServerConf {
public:
    typedef std::pair<unsigned short, std::vector<std::string> >    codeRoutePair;
    struct LocationBlock {
        std::string                 routeName;
        bool                        allowGet;
        bool                        allowPost;
        bool                        allowDelete;
        std::vector<codeRoutePair>  redirRoute;
        std::string                 rootDir;
        bool                        autoIndex;
        std::vector<std::string>    indexArray;
        std::string                 uploadDir;
    };
    struct ServerBlock {
        unsigned short              port;
        in_addr_t                   host;
        std::vector<std::string>    servNames;
        std::vector<codeRoutePair>  defErrorPage;
        unsigned int                maxBytes;
        std::vector<LocationBlock>  locationBlock;
    };
    typedef std::vector<LocationBlock>::const_iterator  locationConstIte;
    typedef std::vector<std::string>::iterator          strVectorIte;
    typedef std::vector<codeRoutePair>::iterator        codeRouteIte;

private:
    ServerBlock serverBlock_;

public:
    //Constructors
    ServerConf();
    ServerConf(const ServerConf &cpy);
    ServerConf& operator=(const ServerConf &cpy);
    explicit ServerConf(const ServerBlock &block);

    //Iterators
    locationConstIte    locationBegin() const;
    locationConstIte    locationEnd() const;

    //Peek
    locationConstIte    findLocation(const std::string &route) const;

    //Modifiers
    void                setPort(unsigned int port);
    void                setHost(const std::string &host);
    void                addServName(const std::string &name);
    void                addErrorPage(unsigned int code, const std::string &route);
    void                setMaxBytes(unsigned long bytes);

    void                addLocation(const LocationBlock &block);
    locationConstIte    addLocationRedir(const std::string &blockRoute, const std::string &route, unsigned int code);
    locationConstIte    addLocationIndex(const std::string &blockRoute, const std::string &index);

    //Getters
    const ServerBlock   &serverBlock() const;
    locationConstIte    locationBlock(const std::string &route) const;

    //Destructor
    ~ServerConf();
};


#endif //WEBSERV_SERVERCONF_HPP
