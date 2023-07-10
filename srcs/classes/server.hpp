#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <list>
#include <map>
#include <sstream>
#include <cctype>
#include <fcntl.h>
#include "socket_manager.hpp"

class Server {

typedef std::map<std::string, std::list<std::string> > KeyValueMap;
typedef KeyValueMap::const_iterator MapIterator;
typedef std::list<std::string>::const_iterator ListIterator;

public:
    explicit Server(int port);
    Server();
    ~Server();
    void start(SocketManager &serverSockets);

private:
    int port;
    int serverSocket;
    std::string fileName;
	std::string data;
	KeyValueMap keyValuePairs;
    int 			createServerSocket();
    void 			bindServerSocket(int serverSocket, int port);
    int 			acceptClientConnection(int serverSocket);
    std::string 	extractBoundary(void);
    bool 			saveFileContent(const std::string& boundary);
    void 			sendResponse(int clientSocket, const std::string &response);
    void 			handleClientRequest(int clientSocket);
	void 			extractValues(const std::string &key, std::string& value);
	void 			extractFilename(void);
	void 			printValueForKey(const std::string& key);
	void 			parseRequest(void);
	void			printMap(void);
	bool 			isASCII(const std::string& str);
	std::string		getRequestedFilename(void);
	std::string		getDeletedFilename(void);
	std::string		loadStaticContent(const std::string &filename);
	std::string		loadStatic(void);
	void			handlePostRequest(int clientSocket);
	void			handleGetRequest(int clientSocket);
	void			handleDeleteRequest(int clientSocket);
	
	void 			crossRoads(int clientSocket);
	void			clearAll(void);


};

#endif

