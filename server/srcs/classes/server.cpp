#include "server.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <fcntl.h>
#include <cstring>
#include <netdb.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/select.h>

Server::Server() {}

Server::~Server() {}

//Client socket handling
int Server::acceptClientConnection(int serverSocket)
{
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket < 0)
    {
        if(errno == EWOULDBLOCK || errno == EAGAIN)
        {
            std::cout << "No pending connections" << std::endl;
            return(-1);
        }

        else
        {
            std::cerr << "Error while trying to accept a new client" << std::endl;
        }
    }
    return clientSocket;
}

void Server::setSockets(fd_set &readfds, fd_set &writefds, SocketManager &serverSockets, std::vector<int> &clientSocket) {
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    for (SocketManager::SockIter it = serverSockets.sockBegin(); it != serverSockets.sockEnd(); it++) {
        FD_SET(it->first, &readfds);
    }

    for (const auto& client : clientSocket)
    {
        FD_SET(client, &readfds);
    }
}

void Server::checkIncoming(fd_set &readfds, SocketManager &serverSockets, std::vector<int> &clientSocket, int &biggest) {
    for (SocketManager::SockIter it = serverSockets.sockBegin(); it != serverSockets.sockEnd(); it++) {
        if (FD_ISSET(it->first, &readfds)) {
            int newClient = acceptClientConnection(it->first);
            if (newClient != -1) {
                fcntl(newClient, F_SETFL, O_NONBLOCK);
                clientSocket.push_back(newClient);
                if (newClient > biggest) {
                    biggest = newClient;
                }
            }
        }
    }
}

void Server::handleClientRequest(int clientSocket, ServerConf &conf)
{
    char c;
    ssize_t bytesRead;
    int contentLength = 0;
    bool foundEndOfHeaders = false;
    std::string endOfHeaders = "\r\n\r\n";
    std::string contentLengthHeader = "Content-Length: ";

    while ((bytesRead = recv(clientSocket, &c, 1, 0)) > 0)
    {
        this->data.push_back(c);
        if (!foundEndOfHeaders)
        {
            if (this->data.find(endOfHeaders) != std::string::npos)
            {
                foundEndOfHeaders = true;
                std::size_t contentLengthPos = this->data.find(contentLengthHeader);
                if (contentLengthPos != std::string::npos)
                {
                    contentLengthPos += contentLengthHeader.length();
                    std::string contentLengthStr = cut(this->data.substr(contentLengthPos), "\r\n");
                    contentLength = std::stoi(contentLengthStr);

                }
            }
        }

        if (foundEndOfHeaders && this->data.length() - this->data.find(endOfHeaders) - endOfHeaders.length() >= contentLength)
        {
            break;
        }
    }
    std::cout << "request is \n" << this->data << std::endl; //TODO Debug

    parseRequest();
    printValueForKey("Content-Type"); //TODO debug
    extractFilename();
    printMap(); //TODO Debug
    crossRoads(clientSocket, conf);
    clearAll();
}

std::string Server::findDirFile(std::string &file, std::string &root, const std::string &location, const ServerConf &conf) {
    std::string response;
    DIR *dir;
    struct dirent *entry;
    ServerConf::LocationConstIterator it = conf.findLocation(location);

    std::cout << "root: >" << root << "<\n";
    std::cout << "location: >" << location << "<\n";
    std::cout << "Gotten Filename: >" << file << "<\n"; //TODO Debug

    root.empty() ? dir = opendir(".") : dir = opendir(root.c_str());
    if (dir != NULL) {
        while ((entry = readdir(dir))) {
            std::string name = entry->d_name;
            if (name == "." || name == "..") {
                continue;
            }
            std::cout << "entry: " << name << "\n";

            if (it != conf.locationConstEnd()) {
                if (!it->indexes.empty()) {
                    std::vector<std::string>::const_iterator index = std::find(it->indexes.begin(),
                                                                               it->indexes.end(), name);
                    if (index != it->indexes.end()) {
                        response = getHTTPCode(conf, "200", root + *index); //TODO test this chiet
                        break ;
                    }
                }
            }
        }
        closedir(dir);
    }

    if (response.empty()) {
        if (it != conf.locationConstEnd() && it->autoIndex) {
            showAutoIndex(root, response);
        } else {
            response = getHTTPCode(conf, "404");
        }
    }
    return response;
}

#include <iostream>
void Server::handleGetRequest(int clientSocket, ServerConf &conf) {
    std::string response;
    std::string file = getRequestedFilename();
    std::string ext = getFileExt(file);

    std::string root;
    root = searchFullRoot(file, conf);
    std::string location = searchFileLocation(file);

    std::cout << "root: " << root << "\n";
    std::cout << "file: " << file << "\n";
    std::cout << "loca: " << location << "\n";
    if (isValidHost(conf)) {
        std::pair<std::string, std::string> content = loadStaticContent(root);
        std::string fileContent = content.second;

        if (ext.empty() || (ext != "sh" && ext != "py" && ext != "pl"
                            && ext != "php" && ext != "rb" && ext != "js")) {
            if (isDirectory(root) || file.empty()) {
                response = findDirFile(file, root, location, conf); //TODO check more
            } else {
                if (fileContent.empty()) {
                    //response = findAltFile(file, conf, location);
                } else {
                    response = getHTTPCode(conf, "200", root);
                }
            }
        } else {
            //TODO CGI here
        }
    } else {
        response = getHTTPCode(conf, "404");
    }
    this->clientResponses.push_back(std::make_pair(clientSocket, response));
}

void    Server::showAutoIndex(std::string &fileName, std::string &response) {
    if (fileName.empty() || fileName == "favicon.ico")
        fileName = ".";
    if(fileName.back() == '/' || fileName == ".")
    {
        std::cout << "DIRECTORY\n\n";
        std::string autoIndex;
        autoIndex += "<html><head><title>Autoindex</title></head><body>";
        autoIndex += "<h1>Index of " + fileName + "</h1>";
        autoIndex += "<ul>";

        DIR *dir;
        struct dirent* entry;
        std::vector<std::string> files;
        std::vector<std::string> directories;

        if((dir = opendir(fileName.c_str())) != NULL)
        {
            while((entry = readdir(dir)) != NULL)
            {
                std::string name = entry->d_name;
                std::string path = fileName += "/" + name;

                if(entry->d_type == DT_DIR)
                {
                    directories.push_back(name);
                }

                else if(entry->d_type == DT_REG)
                {
                    files.push_back(name);
                }
            }
            closedir(dir);
        }

        std::sort(directories.begin(), directories.end());
        std::sort(files.begin(), files.end());

        std::vector<std::string>::const_iterator directoryIt;

        for (directoryIt = directories.begin(); directoryIt != directories.end(); ++directoryIt)
        {
            const std::string &directory = *directoryIt;
            autoIndex += "<li><a href=\"";
            autoIndex += directory + "/";
            autoIndex += "\">";
            autoIndex += directory;
            autoIndex += "/</a></li>";
        }

        std::vector<std::string>::const_iterator fileIt;

        for (fileIt = files.begin(); fileIt != files.end(); ++fileIt)
        {
            const std::string &file = *fileIt;
            autoIndex += "<li><a href=\"";
            autoIndex += file;
            autoIndex += "\">";
            autoIndex += file;
            autoIndex += "</a></li>";
        }

        autoIndex += "</ul>";
        autoIndex += "</body></html>";

        std::ostringstream oss;

        oss << "HTTP/1.1 200 OK\r\n";
        oss << "Content-Type: text/html\r\n";
        oss << "Content-Length: " << autoIndex.length() << "\r\n";
        oss << "\r\n";
        oss << autoIndex;
        response = oss.str();
    }

    else
    {
        response = generateDownloadResponse(fileName);
    }
}

void Server::crossRoads(int clientSocket, ServerConf &conf)
{
    std::string requestMethod = cut(this->data, " ");
    std::cout << "REQUEST METHOD: " << requestMethod << std::endl; //TODO Debug
    if(requestMethod == "GET" && isAllowedMethod(conf, GET))
    {
        handleGetRequest(clientSocket, conf);
    }
    else if(requestMethod == "POST" && isAllowedMethod(conf, POST))
    {
        //handlePostRequest(clientSocket); //TODO
    }
    else if(requestMethod == "DELETE" && isAllowedMethod(conf, DELETE))
    {
        //handleDeleteRequest(clientSocket); //TODO
    }
    else
    {
        std::string response = getHTTPCode(conf, "405");
        this->clientResponses.push_back(std::make_pair(clientSocket, response));
    }
}

void Server::checkClientRequest(fd_set &readfds, fd_set &writefds, SocketManager &serverSockets, std::vector<int> &clientSocket) {
    for (auto it = clientSocket.begin(); it != clientSocket.end();)
    {
        if (FD_ISSET(*it, &readfds))
        {
            SocketManager::SockIter conf = findClientConfig(*it, serverSockets);
            handleClientRequest(*it, conf->second);
            FD_CLR(*it, &readfds);
            FD_SET(*it, &writefds);

            it = clientSocket.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Server::sendResponse(int clientSocket, const std::string& response)
{
    size_t totalBytesSent = 0;
    size_t bytesRemaining = response.length();
    unsigned long bufferSize = response.length();
    int result = setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    if(result == -1) {
        std::cout << "Error while assigning the buffer's size to socket\n" << std::endl;
    }

    while (totalBytesSent < response.length())
    {
        ssize_t bytesSent = send(clientSocket, response.c_str() + totalBytesSent, bytesRemaining, 0);

        if (bytesSent == -1)
        {
            std::cerr << "Error while trying to send the response" << std::endl;
            break;
        }
        else if (bytesSent == 0)
        {
            std::cerr << "The client has closed the connection." << std::endl;
            break;
        }

        totalBytesSent += bytesSent;
        std::cout << "Tamaño de la respuesta " << response.length() << std::endl; //TODO Debug
        std::cout << "Bytes Enviados: " << totalBytesSent << std::endl; //TODO Debug
        bytesRemaining -= bytesSent;
        std::cout << "Bytes Restantes " << bytesRemaining << std::endl; //TODO Debug
    }
}

void Server::handleResponses(fd_set &writefds) {
    for (auto it = this->clientResponses.begin(); it != this->clientResponses.end();)
    {
        if (FD_ISSET(it->first, &writefds))
        {
            int fd = it->first;
            std::string re = it->second;
            sendResponse(fd, re);
            close(fd);
            it = this->clientResponses.erase(it);
            FD_CLR(fd, &writefds);
        }
        else
        {
            ++it;
        }
    }
}

void Server::start(SocketManager &serverSockets) {
    serverSockets.listenOnSock(serverSockets.sockBegin(), serverSockets.sockEnd());
    for (SocketManager::SockIter it = serverSockets.sockBegin(); it != serverSockets.sockEnd(); it++) {
        fcntl(it->first, F_SETFL, O_NONBLOCK);
    }

    fd_set readfds;
    fd_set writefds;
    int biggest = (--serverSockets.sockEnd())->first;

    std::vector<int> clientSocket;
    int result = 0;
    while (true)
    {
        setSockets(readfds, writefds, serverSockets, clientSocket);

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        result = select(biggest + 1, &readfds, &writefds, NULL, &timeout);
        if (result < 0 || result == 0)
        {
            if (result < 0) {
                std::cerr << "select: error: " << strerror(errno) << std::endl;
            }
            continue;
        }

        checkIncoming(readfds, serverSockets, clientSocket, biggest);
        checkClientRequest(readfds, writefds, serverSockets, clientSocket);
        handleResponses(writefds);
    }
    for (SocketManager::SockIter it = serverSockets.sockBegin(); it != serverSockets.sockEnd(); it++) {
        close(it->first);
    }
}

//Parsing
void Server::parseRequest()
{
    std::istringstream iss(this->data);
    std::string line;

    while (std::getline(iss, line))
    {
        if (line.empty() || !isASCII(line))
        {
            continue;
        }

        std::size_t separatorPos = line.find(':');

        if (separatorPos != std::string::npos)
        {
            std::string key = line.substr(0, separatorPos);
            std::string value = line.substr(separatorPos + 1);
            key.erase(0, key.find_first_not_of(' '));
            key.erase(key.find_last_not_of(' ') + 1);
            value.erase(0, value.find_first_not_of(' '));
            value.erase(value.find_last_not_of(' ') + 1);
            this->keyValuePairs[key].push_back(value);
        }
    }
}

void Server::extractFilename()
{
    std::string key = "Content-Disposition";
    MapIterator it = this->keyValuePairs.find(key);
    if (it != this->keyValuePairs.end())
    {
        const std::list<std::string>& values = it->second;
        std::string filenamePrefix = "filename=\"";
        std::string filenameSuffix = "\"";

        std::list<std::string>::const_iterator listIt;
        for (listIt = values.begin(); listIt != values.end(); ++listIt)
        {
            std::size_t prefixPos = listIt->find(filenamePrefix);
            if (prefixPos != std::string::npos)
            {
                std::size_t suffixPos = listIt->find(filenameSuffix, prefixPos + filenamePrefix.length());
                if (suffixPos != std::string::npos)
                {
                    this->fileName = listIt->substr(prefixPos + filenamePrefix.length(), suffixPos - (prefixPos + filenamePrefix.length()));
                    break;
                }
            }
        }

        if (!this->fileName.empty())
        {
            std::cout << "Filename: " << this->fileName << std::endl;
        }
        else
        {
            std::cout << "Filename not found in " << key << std::endl;
        }
    }
    else
    {
        std::cout << "Key not found: " << key << std::endl;
    }
}

std::string Server::getRequestedFilename()
{
    std::string filename;
    std::size_t startPos = this->data.find("GET /") + 5;
    std::size_t endPos = this->data.find(" HTTP/1.1\r\n");
    if (endPos != std::string::npos)
    {
        filename = this->data.substr(startPos, endPos - startPos);
    }
    return filename;
}

std::string Server::generateDownloadResponse(const std::string& filename)
{
    std::string content = readFileContent(filename);

    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Type: " << getContentType(getFileExt(filename)) << "\r\n";
    oss << "Content-Length: " << content.length() << "\r\n";
    oss << "Content-Disposition: attachment; filename=\"" << filename << "\"\r\n";
    oss << "\r\n";
    oss << content;

    return oss.str();
}

std::string Server::readFileContent(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (file)
    {
        std::ostringstream oss;
        oss << file.rdbuf();
        return oss.str();
    }
    return "";
}

//Utils
SocketManager::SockIter Server::findClientConfig(int clientSocket, SocketManager &serverSockets) {
    SocketManager::SockIter pos = serverSockets.sockBegin();
    SocketManager::SockIter defaultServer = serverSockets.sockEnd();
    struct sockaddr_in specs = {};
    socklen_t specs_size = sizeof(specs);

    if (getsockname(clientSocket, (struct sockaddr*)&specs, &specs_size) == -1) {
        std::cout << "Error while trying to retrieve client information.\n";
        return serverSockets.sockEnd();
    }

    char clientHost[NI_MAXHOST];
    unsigned int clientPort = htons(specs.sin_port);
    inet_ntop(AF_INET, &(specs.sin_addr), clientHost, NI_MAXHOST);

    while (pos != serverSockets.sockEnd()) {
        char serverHost[NI_MAXHOST];
        unsigned int serverPort = htons(pos->second.server().ipv4Addr.sin_port);
        inet_ntop(AF_INET, &(pos->second.server().ipv4Addr.sin_addr), serverHost, NI_MAXHOST);

        if (clientPort == serverPort && static_cast<std::string>(clientHost) == static_cast<std::string>(serverHost)) {
            break ;
        } else if (clientPort == serverPort && static_cast<std::string>(serverHost) == "0.0.0.0") {
            defaultServer = pos;
        }
        ++pos;
    }
    if (pos == serverSockets.sockEnd() && defaultServer != serverSockets.sockEnd()) {
        return defaultServer;
    }
    return pos;
}

std::string Server::cut(const std::string& str, const std::string& separator)
{
    std::size_t pos = str.find(separator);
    if (pos != std::string::npos)
    {
        return str.substr(0, pos);
    }
    return str;
}

bool Server::isASCII(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (static_cast<unsigned char>(*it) > 127)
        {
            return false;
        }
    }
    return true;
}

void Server::clearAll()
{
    this->fileName.clear();
    this->data.clear();
    this->keyValuePairs.clear();
}

std::string Server::getHTTPCode(const ServerConf &conf, const std::string &code, const std::string &file) {
    std::string response;
    std::cout << "the file inside errors: " << file << "\n";
    if (code == "200") {
        std::pair<std::string, std::string> content = loadStaticContent(file);
        std::string contentType = content.first;
        std::string fileContent = content.second;

        response = "HTTP/1.1 200 OK\r\nContent-Type: " + contentType
                   + "\r\nContent-Length: " + std::to_string(fileContent.length())
                   + "\r\n\r\n" + fileContent;
    } else {
        std::pair<std::string, std::string> content;
        if (!conf.server().defErrorPage.empty()) {
            for (ServerConf::UshortVecMap::const_iterator it = conf.server().defErrorPage.begin();
                    it != conf.server().defErrorPage.end(); it++) {
                std::cout << "code on iterator: " << it->first << "\n";
                if (it->first == std::atoi(code.c_str())) {
                    content = loadStaticContent(*it->second.begin());
                    std::string contentType = content.first;
                    std::string fileContent = content.second;
                    size_t start = fileContent.find("<title>") + 7;
                    size_t end = fileContent.find("</title>");
                    std::string title = fileContent.substr(start, end - start);
                    response = "HTTP/1.1 " + title
                               + "\r\nContent-Type: " + contentType
                               + "\r\nContent-Length: " + std::to_string(fileContent.length())
                               + "\r\n\r\n" + fileContent;
                    return response;
                }
            }
        }
        content = loadStaticContent("http/default_errors/" + code + ".html");
        std::string contentType = content.first;
        std::string fileContent = content.second;
        size_t start = fileContent.find("<title>") + 7;
        size_t end = fileContent.find("</title>");
        std::string title = fileContent.substr(start, end - start);
        response = "HTTP/1.1 " + title
                   + "\r\nContent-Type: " + contentType
                   + "\r\nContent-Length: " + std::to_string(fileContent.length())
                   + "\r\n\r\n" + fileContent;
    }
    return response;
}

Server::DoubleStrPair Server::loadStaticContent(const std::string& filename)
{
    if (isDirectory(filename))
    {
        std::cerr << "Couldn't open file: " << filename << std::endl;
        return std::make_pair("", "");
    }

    std::ifstream inputFile(filename, std::ios::binary);

    if (!inputFile)
    {
        std::cerr << "Error while trying to open: " << filename << std::endl;
        return std::make_pair("", "");
    }

    std::string extension = getFileExtension(filename);
    std::string contentType = "text/html";

    if (extension == ".jpg" || extension == ".jpeg")
    {
        contentType = "image/jpeg";
    }
    else if (extension == ".png")
    {
        contentType = "image/png";
    }

    std::string content((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();
    return std::make_pair(contentType, content);
}

bool Server::isDirectory(const std::string &fName)
{
    DIR* dir = opendir(fName.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    return false;
}

std::string Server::getFileExtension(const std::string& filename)
{
    size_t dotIndex = filename.find_last_of('.');
    if (dotIndex != std::string::npos)
    {
        return filename.substr(dotIndex);
    }
    return "";
}

std::string Server::getFileExt(const std::string &filename)
{
    size_t dotIndex = filename.find_last_of('.');

    if (dotIndex != std::string::npos && dotIndex < filename.size() - 1)
    {
        size_t questionMarkIndex = filename.find_last_of('?');
        if (questionMarkIndex != std::string::npos && questionMarkIndex > dotIndex)
        {
            return filename.substr(dotIndex + 1, questionMarkIndex - dotIndex - 1);
        }
        else
        {
            return filename.substr(dotIndex + 1);
        }
    }
    return "";
}

bool Server::isValidHost(const ServerConf &conf) {
    std::string key = "Host";
    MapIterator it = this->keyValuePairs.find(key);

    if (it == this->keyValuePairs.end() || *it->second.begin() != "localhost\r") {
        return false;
    } else {
        if (*it->second.begin() == "localhost\r") {
            return true;
        }
        for (std::vector<std::string>::const_iterator it2 = conf.server().servNames.begin();
             it2 != conf.server().servNames.end(); it2++) {
            if ((*it2 + "\r") == *it->second.begin()) {
                return true;
            }
        }
    }
    return false;
}

bool Server::isAllowedMethod(const ServerConf &conf, int method) {
    std::string file = getRequestedFilename();
    ServerConf::LocationConstIterator location = conf.findLocation(file);

    if (location != conf.locationConstEnd()) {
        switch (method) {
            case GET:
                return location->allowGet;
            case POST:
                return location->allowPost;
            case DELETE:
                return location->allowDelete;
        }
    } else {
        switch (method) {
            case GET:
                return true;
            case POST:
                return false;
            case DELETE:
                return false;
        }
    }
    return false;
}

std::string Server::getContentType(const std::string& extension) {
    if (extension == "html" || extension == "htm") {
        return "text/html";
    } else if (extension == "txt") {
        return "text/plain";
    } else if (extension == "css") {
        return "text/css";
    } else if (extension == "js") {
        return "application/javascript";
    } else if (extension == "jpg" || extension == "jpeg") {
        return "image/jpeg";
    } else if (extension == "png") {
        return "image/png";
    } else if (extension == "gif") {
        return "image/gif";
    } else if (extension == "pdf") {
        return "application/pdf";
    }

    return "application/octet-stream";
}

std::string Server::searchFullRoot(const std::string &file, ServerConf &conf) {
    std::string fullRoot;

    if (!conf.server().rootDir.empty()) {
        fullRoot = conf.server().rootDir;
    }
    if (conf.findLocation("/") != conf.locationConstEnd() && !conf.findLocation("/")->rootDir.empty()) {
        fullRoot = conf.findLocation("/")->rootDir;
    }
    if (file.empty()) {
        return fullRoot;
    }

    std::istringstream is(file);
    std::string line;
    while (std::getline(is, line, '/')) {
        ServerConf::LocationConstIterator it = conf.findLocation(line + '/');
        if (it != conf.locationConstEnd() && !it->rootDir.empty()) {
            fullRoot += it->rootDir;
        } else {
            fullRoot += line + '/';
        }
    }

    if (file.at(file.length() - 1) != '/' && fullRoot.at(fullRoot.length() - 1) == '/') {
        fullRoot.erase(fullRoot.length() - 1);
        fullRoot.erase(fullRoot.find_last_of('/') + 1);
        fullRoot += file.substr(file.find_last_of('/') + 1);
    } else if (file.at(file.length() - 1) == '/' && fullRoot.at(fullRoot.length() - 1) != '/') {
        fullRoot += '/';
    }
    return fullRoot;
}

std::string Server::searchFileLocation(const std::string &file) {
    std::string location = file;

    if (!location.empty()) {
        if (location.find_last_of('/') != std::string::npos) {
            location.erase(location.find_last_of('/'));
            location.erase(0, location.find_last_of('/') + 1);
        } else {
            location.erase(0, location.length());
        }
    }
    location += '/';
    return location;
}

//Debug
void Server::printValueForKey(const std::string& key)
{
    MapIterator it = this->keyValuePairs.find(key);
    if (it != this->keyValuePairs.end())
    {
        const std::list<std::string>& values = it->second;
        std::cout << key << ": ";
        ListIterator listIt;
        for (listIt = values.begin(); listIt != values.end(); ++listIt)
        {
            std::cout << *listIt << std::endl;
        }
        std::cout << std::endl;
    }

    else
    {
        std::cout << "Key not found: " << key << std::endl;
    }
}

void Server::printMap()
{
    MapIterator it;
    ListIterator listIt;

    for (it = this->keyValuePairs.begin(); it != this->keyValuePairs.end(); ++it)
    {
        std::cout << "Key: " << it->first << std::endl;
        std::cout << "Values:" << std::endl;

        for (listIt = it->second.begin(); listIt != it->second.end(); ++listIt)
        {
            std::cout << "- " << *listIt << std::endl;
        }

        std::cout << std::endl;
    }
}
