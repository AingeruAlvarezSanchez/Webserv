#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP
#include <exception>
#include <string>

class ServerConf {
public:

    //Error handling
    class BadSyntax : public std::exception {
    private:
        char const* _error;
    public:
        BadSyntax(char const *msg) : _error(msg) {}

        char const* what() const throw() {
            return _error;
        }
    };
private:
};

#endif //SERVER_CONF_HPP
