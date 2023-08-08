#ifndef WEBSERV_SERVERCONF_HPP
#define WEBSERV_SERVERCONF_HPP

#include <arpa/inet.h>
#include <vector>
#include <map>
#include <string>
#include <ostream>
#include <climits>

#define GET     0
#define POST    1
#define DELETE  2

class ServerConf {
public:
    //Primitive data types definitions
    typedef unsigned short  ushort_t;
    typedef unsigned int    uint_t;
    typedef unsigned short  ulong_t;

    //Non Class-dependant containers definitions
    typedef std::vector<std::string>                        StrVector;
    typedef std::map<ushort_t, StrVector>                   UshortVecMap;
    typedef std::pair<ushort_t, std::vector<std::string> >  UshortVecPair;
    typedef std::pair<ushort_t, std::string>                UshortStrPair;

    struct LocationBlock {
        std::string                 path;
        std::string                 rootDir;
        bool                        allowGet;
        bool                        allowPost;
        bool                        allowDelete;
        UshortStrPair               redirect;
        UshortVecPair               try_files;
        bool                        autoIndex;
        StrVector                   indexes;
        StrVector                   cgiLangs;
        std::string                 uploadDir;
    };

    //Location struct and class-dependant definitions
    typedef std::vector<LocationBlock>                  LocationVector;
    typedef std::vector<LocationBlock>::iterator        LocationIterator;
    typedef std::vector<LocationBlock>::const_iterator  LocationConstIterator;

    struct ServerBlock {
        sockaddr_in                 ipv4Addr;
        sockaddr_in6                ipv6Addr;
        StrVector                   servNames;
        std::string                 rootDir;
        UshortVecMap                defErrorPage;
        uint_t                      maxBytes;
        LocationVector              locationBlock;
    };
private:
    ServerBlock serverBlock_;

public:
    //Constructors
    ServerConf();
    ServerConf(const ServerConf &cpy);
    explicit ServerConf(const ServerBlock &block);
    ServerConf& operator=(const ServerConf &cpy);

    //Iterators
    LocationIterator        locationBegin();
    LocationIterator        locationEnd();
    LocationConstIterator   locationConstBegin() const;
    LocationConstIterator   locationConstEnd() const;

    //Peek
    LocationIterator        findLocation(const std::string &path);
    LocationConstIterator   findLocation(const std::string &path) const;

    //Server Block Modifiers
    void                setPort(uint_t port);
    void                setHost(const std::string &host, ushort_t domain);
    void                addServName(const std::string &name);
    void                addErrorPage(uint_t code, const std::string &path);
    void                setMaxBytes(uint_t bytes);
    void                eraseServName(const std::string &name);
    void                eraseServName(const StrVector::iterator &begin, const StrVector::iterator &end);
    LocationIterator    setRootDir(const std::string &name, const std::string &dst = "");
    void                eraseErrorPage(uint_t code, const std::string &name);
    void                eraseErrorPage(uint_t code, const StrVector::iterator &begin, const StrVector::iterator &end);

    //Location Block Modifiers
    LocationIterator    addLocation(const LocationBlock &block);
    LocationIterator    setLocationPath(const std::string &name, const std::string &dst = "");
    LocationIterator    flipPermissions(uint_t method, const std::string& dst);
    LocationIterator    addLocationRedir(uint_t code, const std::string &path, const std::string &dst);
    LocationIterator    addLocationAltFile(uint_t code, const std::string &path, const std::string &dst);
    LocationIterator    setAutoIndex(bool value, const std::string &dst);
    LocationIterator    addLocationIndex(const std::string &index, const std::string &dst);
    LocationIterator    addLocationCgi(const std::string &cgi, const std::string &dst);
    LocationIterator    setUploadDir(const std::string& path, const std::string &dst);
    void                clearLocations();

    //Getters
    const ServerBlock   &server() const;

    //Destructor
    ~ServerConf();
};

std::ostream    &operator<<(std::ostream &os, const ServerConf &serv);

#endif //WEBSERV_SERVERCONF_HPP
