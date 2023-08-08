#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
//#include <sys/event.h> --->>  only for MacOs
#include <sys/types.h>
#include <sys/time.h>
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
#include <dirent.h>
#include <vector>
#include <algorithm>
#include "socket_manager.hpp"

class Server {

typedef std::map<std::string, std::list<std::string> > KeyValueMap;
typedef KeyValueMap::const_iterator MapIterator;
typedef std::list<std::string>::const_iterator ListIterator;
typedef std::pair<int, std::string> Pair;

public:
    explicit Server(int port);
    Server();
    ~Server();
    void start(SocketManager &serverSockets);

private:
    std::string path;
    int port;
    std::string fileName;
	std::string data;
	KeyValueMap keyValuePairs;
	std::vector<Pair> clientResponses;
    int 			acceptClientConnection(int serverSocket);
    std::string 	extractBoundary(void);
    bool 			saveFileContent(const std::string& boundary);
	void 			sendResponse(int clientSocket, const std::string &response);
    void 			handleClientRequest(int clientSocket, ServerConf &conf);
	void 			extractValues(const std::string &key, std::string& value);
	void 			extractFilename(void);
	void 			printValueForKey(const std::string& key);
	void 			parseRequest(void);
	void			printMap(void);
	bool 			isASCII(const std::string& str);
	std::string		getRequestedFilename(void);
	std::string		getDeletedFilename(void);
//	std::string		loadStaticContent(const std::string &filename);
	std::pair<std::string, std::string>	loadStaticContent(const std::string& filename);
	bool			isDirectory(const std::string &fileName);
	std::string		loadStatic(void);
	void			handlePostRequest(int clientSocket);
	void			handleGetRequest(int clientSocket, ServerConf &conf);
	void			handleDeleteRequest(int clientSocket);
    void            assignCGIResponse(std::string &response);
	
	void 			crossRoads(int clientSocket, ServerConf &conf);
	void			clearAll(void);


};

#endif

