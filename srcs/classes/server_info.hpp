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

    typedef std::pair< std::string, s_location >                                 locationDirective;
    typedef std::vector< std::pair< std::string, s_location > >                  configuredLocations;
    typedef std::vector< std::pair< std::string, s_location > >::const_iterator  configuredLocationsIterator;

    typedef struct {
        unsigned short              serverPort;
        std::vector< std::string >  allowedHosts;
        std::vector< std::string >  serverNames;
        std::string                 errorPageRoute;
        unsigned int                maxBodyBytes;
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

    //Setters
    void    setServerData(s_serverData const& value);

    template<typename T>
    static void    set_listen_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "listen>" << line;
    }

    template<typename T>
    static void    set_server_name_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "server_name>" << line;
    }

    template<typename T>
    static void    set_error_page_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "error_page>" << line;
    }

    template<typename T>
    static void    set_client_max_body_size_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "client_max_body_size>" << line;
    }

    template<typename T>
    static void    set_limit_except_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "limit_except>" << line;
    }

    template<typename T>
    static void    set_return_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "return>" << line;
    }

    template<typename T>
    static void    set_root_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "root>" << line;
    }

    template<typename T>
    static void    set_try_files_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "try_files>" << line;
    }

    template<typename T>
    static void    set_auto_index_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "auto_index>" << line;
    }

    template<typename T>
    static void    set_index_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "index>" << line;
    }

    template<typename T>
    static void    set_cgi_pass_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "cgi_pass>" << line;
    }

    template<typename T>
    static void    set_upload_rule(std::string const& line,__attribute_maybe_unused__ T & directive) {
        std::cout << "upload>" << line;
    }

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
