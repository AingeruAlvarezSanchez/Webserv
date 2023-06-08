#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP
#include <exception>
#include <string>
#include <vector>
#include <map>

class ServerConf {
public:
    typedef struct {
        int                         socketFd;
        //getsockname()
        //TODO location
    }   confValuesData;

private:
    std::vector< confValuesData >   _socketsConfData;
public:
    //Constructors
    ServerConf();
    ServerConf(std::vector< confValuesData > const& socketsConfigs);
    ServerConf(ServerConf const& cpy);

    //Destructor
    ~ServerConf();

    //Operator overloads
    ServerConf& operator=(ServerConf const& cpy);

    //Data management
    int createSocketConf(std::ifstream const& file); //TODO

    //JSON Configuration
    bool    serverJSONCreation(); //TODO

    //Getters
    std::vector< ServerConf::confValuesData >::const_iterator  getSocketConfData(int socketFd) const;

    //TODO create .last function

    //Configuration files error handling
    class BadSyntax : public std::exception {
    private:
        char const* _error;
    public:
        BadSyntax(char const *msg) : _error(msg) {}

        char const* what() const throw() {
            return _error;
        }
    };
};

#endif //SERVER_CONF_HPP
