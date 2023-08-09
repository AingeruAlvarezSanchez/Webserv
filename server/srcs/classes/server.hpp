#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <list>
#include <map>
#include "socket_manager.hpp"


class Server {

    typedef std::map<std::string, std::list<std::string> >  KeyValueMap;
    typedef KeyValueMap::const_iterator                     MapIterator;
    typedef std::list<std::string>::const_iterator          ListIterator;
    typedef std::pair<int, std::string>                     Pair;
    typedef std::pair<std::string, std::string>             DoubleStrPair;

public:

    Server();
    ~Server();
    void start(SocketManager &serverSockets);

private:
    //Attributes
    std::string         data;
    std::string         fileName;
    KeyValueMap         keyValuePairs;
    std::vector<Pair>   clientResponses;

    //Client socket handling
    int     acceptClientConnection(int serverSocket);
    void    setSockets(fd_set &readfds, fd_set &writefds, SocketManager &serverSockets, std::vector<int> &clientSocket);
    void    checkIncoming(fd_set &readfds, SocketManager &serverSockets, std::vector<int> &clientSocket, int &biggest);
    void    checkClientRequest(fd_set &readfds, fd_set &writefds, SocketManager &serverSockets, std::vector<int> &clientSocket);
    void    handleClientRequest(int clientSocket, ServerConf &conf);
    void    handleResponses(fd_set &writefds);
    void    sendResponse(int clientSocket, const std::string& response);
    void    crossRoads(int clientSocket, ServerConf &conf);
    void    handleGetRequest(int clientSocket, ServerConf &conf);
    void    showAutoIndex(std::string &fileName, std::string &response);

    //Parsing
    void        parseRequest();
    void        extractFilename();
    std::string getRequestedFilename();
    std::string generateDownloadResponse(const std::string& filename);
    std::string readFileContent(const std::string& filename);

    //Utils
    static SocketManager::SockIter  findClientConfig(int clientSocket, SocketManager &serverSockets);
    static std::string              cut(const std::string& str, const std::string& separator);
    static bool                     isASCII(const std::string& str);
    void                            clearAll();
    static std::string              getHTTPCode(const ServerConf &conf, const std::string &code, const std::string &file = std::string());
    static DoubleStrPair            loadStaticContent(const std::string& filename);
    static bool                     isDirectory(const std::string &fName);
    static std::string              getFileExtension(const std::string& filename);
    static std::string              getFileExt(const std::string& filename);
    bool                            isValidHost(const ServerConf &conf);
    bool                            isAllowedMethod(const ServerConf &conf, int method);
    std::string                     getContentType(const std::string& extension);
    static std::string              searchFullRoot(const std::string &file, ServerConf &conf);
	std::string						findDirFile(std::string &file, const ServerConf &conf);

    //Debug
    void    printValueForKey(const std::string& key);
    void    printMap();

};

#endif //SERVER_HPP
