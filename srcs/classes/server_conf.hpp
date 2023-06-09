#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP
#include <exception>
#include <string>
#include <vector>
#include <map>

class ServerConf {
public:
    typedef struct {
        int socketFd; // TODO array of INTS / To calculate size of an int * => sizeof(socketFds) / sizeof(socketFds)[0]
        //getsockname()
        //TODO location
    }   socketParams;
    typedef std::vector< socketParams >::const_iterator   SocketParamsIterator;
private:
    std::vector< socketParams >   _socketsConfData;
public:
    //Constructors
    ServerConf();
    explicit ServerConf(std::vector< socketParams > const& socketsConfigs);
    ServerConf(ServerConf const& cpy);

    //Destructor
    ~ServerConf();

    //Operator overloads
    ServerConf& operator=(ServerConf const& cpy);

    //Data management
    int createSocketConf(std::ifstream const& file); //TODO

    //JSON Configuration
    bool    serverJSONCreation() const; //TODO

    //Getters
    SocketParamsIterator  getSocketConfData(int socketFd) const;

    bool  invalidSocket(SocketParamsIterator const& it) const;

    //Configuration files error handling
    class BadSyntax : public std::exception {
    private:
        char const* _error;
    public:
        explicit BadSyntax(char const *msg) : _error(msg) {}

        char const* what() const throw() {
            return _error;
        }
    };
};

#endif //SERVER_CONF_HPP
