#ifndef WEBSERV_SERVERCONF_HPP
#define WEBSERV_SERVERCONF_HPP

#include <arpa/inet.h>
#include <vector>
#include <map>
#include <string>

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
    typedef std::map<ushort_t, std::vector<std::string> >   UshortVecMap;
    typedef std::vector<std::string>                        StrVector;

    struct LocationBlock {
        std::string                 path;
        bool                        allowGet;
        bool                        allowPost;
        bool                        allowDelete;
        UshortVecMap                lo_redirs;
        std::string                 rootDir;
        bool                        autoIndex;
        StrVector                   lo_indexes;
        std::string                 uploadDir;
    };

    //Location struct and class-dependant definitions
    typedef std::vector<LocationBlock>                  LocationVector;
    typedef std::vector<LocationBlock>::iterator        LocationIterator;
    typedef std::vector<LocationBlock>::const_iterator  LocationConstIterator;

    struct ServerBlock {
        ushort_t                    port;
        struct in_addr              ipv4Host;
        struct in6_addr             ipv6Host;
        StrVector                   servNames;
        UshortVecMap                defErrorPage;
        uint_t                      maxBytes;
        UshortVecMap                se_redirs;
        StrVector                   se_indexes;
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
    void    setPort(uint_t port);
    void    setHost(const std::string &host, ushort_t af);
    void    addServName(const std::string &name);
    void    addErrorPage(uint_t code, const std::string &path);
    void    setMaxBytes(ulong_t bytes);
    void    eraseServName(const std::string &name);
    void    eraseServName(const StrVector::iterator &begin, const StrVector::iterator &end);
    void    eraseErrorPage(uint_t code, const std::string &name);
    void    eraseErrorPage(uint_t code, const StrVector::iterator &begin, const StrVector::iterator &end);

    //Location Block Modifiers
    LocationIterator    addLocation(const LocationBlock &block);
    LocationIterator    setLocationPath(const std::string &name, const std::string &dst = "");
    LocationIterator    flipPermissions(uint_t method, const std::string& dst);
    LocationIterator    addlocationRedir(uint_t code, const std::string &path, const std::string &dst);
    LocationIterator    setRootDir(const std::string &name, const std::string &dst);
    LocationIterator    setAutoIndex(bool value, const std::string &dst);
    LocationIterator    addLocationIndex(const std::string &index, const std::string &dst);
    LocationIterator    setUploadDir(const std::string& path, const std::string &dst);
    void                eraseLocation(const std::string &dst);
    void                eraseLocation(const LocationIterator &begin, const LocationIterator &end);
    void                clearLocations();

    //General modifiers
    void    eraseRedir(uint_t code, const std::string &name, const std::string &dst = "");
    void    eraseIndex(const std::string &name, const std::string &dst = "");

    template<typename Iterator>
    void    eraseRedir(uint_t code, const Iterator &begin, const Iterator &end, const std::string &dst = "") {
        if (!dst.empty()) {
            LocationIterator it = findLocation(dst);
            if (it != locationEnd()) {
                UshortVecMap::iterator redirIt = it->lo_redirs.find(code);
                redirIt->second.erase(begin, end);
            }
        } else {
            UshortVecMap::iterator it = serverBlock_.se_redirs.find(code);
            it->second.erase(begin, end);
        }
    };

    template<typename Iterator>
    void    eraseIndex(const Iterator &begin, const Iterator &end, const std::string &dst = "") {
        if (!dst.empty()) {
            LocationIterator it = findLocation(dst);
            if (it != locationEnd()) {
                it->lo_indexes.erase(begin, end);
            }
        }
        else {
            serverBlock_.se_indexes.erase(begin, end);
        }
    };

    //Getters
    const ServerBlock   &server() const;

    //Destructor
    ~ServerConf();
};


#endif //WEBSERV_SERVERCONF_HPP
