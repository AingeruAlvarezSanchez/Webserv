#include "og_server.hpp"
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
//  #include <sys/event.h>    ONLY FOR MacOS
#include <sys/time.h>
#include <sys/wait.h>

# define RECV_SIZE 65536

std::string parseChunkedRequest(const char* request) {
  std::string data;

  // Skip the request headers
  const char* start = strstr(request, "\r\n\r\n");
  if (start == NULL) {
    return data; // Invalid request
  }
  start += 4;

  // Process each chunk
  while (true) {
    char* endPtr;
    unsigned long chunkSize = strtoul(start, &endPtr, 16);
    if (chunkSize == 0) {
      break; // Last chunk
    }

    start = endPtr + 2; // Skip chunk size and CRLF

    // Append chunk data to the result
    data.append(start, chunkSize);

    start += chunkSize + 2; // Skip chunk data and CRLF
  }

  return data;
}


std::string getFileExtension(const std::string& filename)
{
    size_t dotIndex = filename.find_last_of(".");
    if (dotIndex != std::string::npos)
    {
        return filename.substr(dotIndex);
    }
    return "";
}


std::string cut(const std::string& cadena, const std::string& separador) 
{
    std::size_t pos = cadena.find(separador);
    if (pos != std::string::npos) 
    {
        return cadena.substr(0, pos);
    }
    return cadena;
}

bool Server::isDirectory(const std::string &fileName)
{
	DIR* dir = opendir(fileName.c_str());
	if (dir)
	{
        closedir(dir);
        return true;
	}

    return false;
}



void Server::printMap(void)
{
	MapIterator it;
	ListIterator listIt;
	
	for (it = this->keyValuePairs.begin(); it != this->keyValuePairs.end(); ++it) 
	{
		std::cout << "Clave: " << it->first << std::endl;
		std::cout << "Valores:" << std::endl;
		
		for (listIt = it->second.begin(); listIt != it->second.end(); ++listIt) 
		{
			std::cout << "- " << *listIt << std::endl;
        }
		
		std::cout << std::endl;
	}
}

Server::Server() {}

Server::~Server()
{
}

std::string Server::getDeletedFilename(void)
{
	std::string filename;
	std::size_t startPos = this->data.find("DELETE /") + 8;
	std::size_t endPos = this->data.find(" HTTP/1.1\r\n");
	if (endPos != std::string::npos)
	{
		filename = this->data.substr(startPos, endPos - startPos);
	}
	return filename;
}

std::string Server::getRequestedFilename(void) 
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

/*
std::string Server::loadStaticContent(const std::string& filename) 
{
	std::ifstream inputFile(filename, std::ios::binary);
	if (!inputFile) 
	{
		std::cerr << "Error al abrir el archivo: " << filename << std::endl;
		return "";
	}
	
	std::string content((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	inputFile.close();
	return content;
}

*/
std::pair<std::string, std::string> Server::loadStaticContent(const std::string& filename)
{
	if (isDirectory(filename))
	{
		std::cerr << "No se puede abrir el archivo: " << filename << std::endl;
		return std::make_pair("", "");
	}

	std::ifstream inputFile(filename, std::ios::binary);

	if (!inputFile)
	{
		std::cerr << "Error al abrir el archivo: " << filename << std::endl;
		return std::make_pair("", ""); // Archivo no válido, retorna un par vacío
	}

    std::string extension = getFileExtension(filename);
    std::string contentType = "text/html"; // Tipo de contenido predeterminado para archivos HTML

    if (extension == ".jpg" || extension == ".jpeg")
    {
        contentType = "image/jpeg";
    }
    else if (extension == ".png")
    {
        contentType = "image/png";
    }
    // Agrega más extensiones y tipos de contenido según tus necesidades

    std::string content((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();
    return std::make_pair(contentType, content);
}

std::string Server::loadStatic(void) 
{
	std::ifstream inputFile("index.html", std::ios::binary);
	if (!inputFile) 
	{
		std::cerr << "Error al abrir el archivo: " << std::endl;
		return "";
	}
	std::string content((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	inputFile.close();
	return content;
}

SocketManager::SockIter findClientConfig(int clientSocket, SocketManager &serverSockets) {
    SocketManager::SockIter pos = serverSockets.sockBegin();
    SocketManager::SockIter defaultServer = serverSockets.sockEnd();
    struct sockaddr_in specs {};
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

/* void Server::start(SocketManager &serverSockets)    /////ONLY FOR LINUX/////
{
    int serverSocket = serverSockets.listenOnSock(serverSockets.sockBegin());
	fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	fd_set readfds;
	fd_set writefds;
	
//	FD_ZERO(&readfds);
//	FD_ZERO(&writefds);
//	FD_SET(serverSocket, &readfds);
	
	int biggest = serverSocket;
	
	std::vector<int> clientSocket;
	int result = 0;
	while (true)
    {
	    FD_ZERO(&readfds);
	    FD_ZERO(&writefds);
	    FD_SET(serverSocket, &readfds);
	    struct timeval timeout;
		while (result == 0)
		{
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			
			result = select(biggest + 1, &readfds, &writefds, NULL, NULL);
		}
		if(FD_ISSET(serverSocket, &readfds))
		{
			int newClient = acceptClientConnection(serverSocket);
			
			if(newClient != -1)
			{
///				fcntl(newClient, F_SETFL, O_NONBLOCK);
				clientSocket.push_back(newClient);
				FD_SET(clientSocket.back(), &readfds);
				if (clientSocket.back() > biggest) {
					biggest = clientSocket.back();
					//TODO
				}
			}

		}
		for (std::vector<int>::iterator it = clientSocket.begin(); it != clientSocket.end(); it) {
			if(FD_ISSET(*it, &readfds))
			{
				
				handleClientRequest(*it);
				FD_CLR(*it, &readfds);
				FD_SET(*it, &writefds);

				it = clientSocket.erase(it);
				
			}
		}


		for (std::vector<Pair>::iterator it = this->clientResponses.begin(); it != this->clientResponses.end();it) 
		{
			if(FD_ISSET(it->first, &writefds))
			{
			
				std::cout << "JELOU ESTOY AQUI\n";
				int fd = it->first;
				std::string re = it->second;
				sendResponse(fd, re);
				std::cout << "JELOU AGAIN\n";
				close(fd);
				it = this->clientResponses.erase(it);
				FD_CLR(it->first, &writefds);
			}
				
			
		}

	}
	
	close(serverSocket);
}*/

void Server::start(SocketManager& serverSockets)   /////ONLY FOR MacOS////
{
    serverSockets.listenOnSock(serverSockets.sockBegin(), serverSockets.sockEnd());
    for (SocketManager::SockIter it = serverSockets.sockBegin(); it != serverSockets.sockEnd(); it++) {
        fcntl(it->first, F_SETFL, O_NONBLOCK);
    }

    fd_set readfds;
    fd_set writefds;

    int biggest = (--serverSockets.sockEnd())->first;

    SocketManager::SockIter conf;
    std::vector<int> clientSocket;
    int result = 0;
    while (true)
    {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        for (SocketManager::SockIter it = serverSockets.sockBegin(); it != serverSockets.sockEnd(); it++) {
            FD_SET(it->first, &readfds);
        }

        for (const auto& client : clientSocket)
        {
            FD_SET(client, &readfds);
        }

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        result = select(biggest + 1, &readfds, &writefds, NULL, &timeout);

        if (result < 0)
        {
            // Error en select
            std::cerr << "Error en select" << std::endl;
            break;
        }
        else if (result == 0)
        {
            // No hay actividad en los descriptores de archivo
            continue;
        }

        for (SocketManager::SockIter it = serverSockets.sockBegin(); it != serverSockets.sockEnd(); it++) {
            if (FD_ISSET(it->first, &readfds)) {
                int newClient = acceptClientConnection(it->first);
                /*SocketManager::SockIter conf = findClientConfig(newClient, serverSockets);
                if (!isAllowedDomain(newClient, *conf)) {
                    //Close connection of the client, send 402:forbidden?
                    close(newClient);
                    newClient = -1;
                }*/ //TODO check here if we can get the domain name of the client
                if (newClient != -1) {
                    fcntl(newClient, F_SETFL, O_NONBLOCK);
                    clientSocket.push_back(newClient);
                    if (newClient > biggest) {
                        biggest = newClient;
                    }
                }
            }
        }

        for (std::vector<int>::iterator it = clientSocket.begin(); it != clientSocket.end();)
        {
            if (FD_ISSET(*it, &readfds))
            {
                conf = findClientConfig(*it, serverSockets);
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

    for (SocketManager::SockIter it = serverSockets.sockBegin(); it != serverSockets.sockEnd(); it++) {
        close(it->first);
    }
}

int Server::acceptClientConnection(int serverSocket)
{
	int clientSocket = accept(serverSocket, nullptr, nullptr);
	if (clientSocket < 0) 
	{
		if(errno == EWOULDBLOCK || errno == EAGAIN)
		{
			std::cout << "no pending connections\n";
			return(-1);
		}

		else
		{
			std::cerr << "Error al aceptar la conexión" << std::endl;
			close(serverSocket);
			exit(1);
		}
	}
	return clientSocket;
}


std::string Server::extractBoundary(void) 
{
	std::string boundary;
	std::size_t boundaryPos = this->data.find("boundary=");
	if (boundaryPos != std::string::npos) 
	{
		std::size_t startPos = boundaryPos + 9;
		std::size_t endPos = this->data.find("\r\n", startPos);
		if (endPos != std::string::npos) 
		{
			boundary = this->data.substr(startPos, endPos - startPos);
		}
	}
	return boundary;
}

bool Server::saveFileContent(const std::string& boundary) 
{
	std::string fileBoundaryStart = "--" + boundary + "\r\n";
	std::string fileBoundaryEnd = "--" + boundary + "--";
	std::size_t fileStartPos = this->data.find(fileBoundaryStart);
	std::size_t fileEndPos = this->data.find(fileBoundaryEnd, fileStartPos);
	
	if (fileStartPos != std::string::npos && fileEndPos != std::string::npos) 
	{
		std::string fileContent;
		std::size_t fileContentStartPos = this->data.find("\r\n\r\n", fileStartPos);
		if (fileContentStartPos != std::string::npos) 
		{
			fileContentStartPos += 4;
			fileContent = this->data.substr(fileContentStartPos, fileEndPos - fileContentStartPos - 2);
			
			std::ofstream outputFile(this->fileName);
			outputFile << fileContent;
			outputFile.close();
			std::cout << "Contenido del archivo guardado exitosamente." << std::endl;
            return true;
        } 
		else 
		{
			std::cerr << "No se pudo encontrar el contenido del archivo." << std::endl;
		}
	} 
	
	else 
	{
		std::cerr << "No se pudo encontrar el archivo en la solicitud." << std::endl;
    }
	return false;
}

/*
void Server::sendResponse(int clientSocket, const std::string& response) 
{
	ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
	if (bytesSent != response.length()) 
	{
		std::cerr << "Error al enviar la respuesta al cliente." << std::endl;
	}

}*/

void Server::sendResponse(int clientSocket, const std::string& response)
{
    size_t totalBytesSent = 0;
    size_t bytesRemaining = response.length();
    int bufferSize = response.length();
    int result = setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    if(result == -1)
	    std::cout << "Error al asignar tamaño de buffer al socket\n" << std::endl;
    else
	    std::cout << "Buffer de salida asignado correctamente en el fd " << clientSocket << " tamaño: " << bufferSize << std::endl;

    while (totalBytesSent < response.length())
    {
        ssize_t bytesSent = send(clientSocket, response.c_str() + totalBytesSent, bytesRemaining, 0);

        if (bytesSent == -1)
        {
            std::cerr << "Error al enviar la respuesta al cliente." << std::endl;
            break;
        }
        else if (bytesSent == 0)
        {
            std::cerr << "La conexión se ha cerrado por parte del cliente." << std::endl;
            break;
        }

        totalBytesSent += bytesSent;
        std::cout << "Tamaño de la respuesta " << response.length() << std::endl;
        std::cout << "Bytes Enviados: " << totalBytesSent << std::endl;
        bytesRemaining -= bytesSent;
        std::cout << "Bytes Restantes " << bytesRemaining << std::endl;
    }
}

void Server::handleClientRequest(int clientSocket, ServerConf &conf)
{
	char c;
//	char	buffer[RECV_SIZE] = {0};
	ssize_t bytesRead;
	int contentLength = 0;
	bool foundEndOfHeaders = false;
	std::string endOfHeaders = "\r\n\r\n";
	std::string contentLengthHeader = "Content-Length: ";
	
	while ((bytesRead = recv(clientSocket, &c, 1, 0)) > 0) 
	{
		this->data.push_back(c);
//		this->data.append(buffer);
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
	std::cout  << "request es \n" << this->data << std::endl;

	parseRequest();
	printValueForKey("Content-Type");
	extractFilename();
	printMap();
	crossRoads(clientSocket, conf);
	clearAll();
}

void Server::clearAll(void)
{
	this->fileName.clear();
	this->data.clear();
	this->keyValuePairs.clear();
}

void Server::crossRoads(int clientSocket, ServerConf &conf)
{
	std::string requestMethod = cut(this->data, " ");

	std::cout << "REQUEST METHOD: " << requestMethod << std::endl;
    if(requestMethod == "GET")
    {
        handleGetRequest(clientSocket, conf);
    }
    else if(requestMethod == "POST")
    {
        handlePostRequest(clientSocket);
    }
    else if(requestMethod == "DELETE")
    {
        handleDeleteRequest(clientSocket);
    }
    else
    {
        std::pair<std::string, std::string> content = loadStaticContent("405.html");
        std::string contentType = content.first;
        std::string fileContent = content.second;
        std::string response = "HTTP/1.1 405 Not allowed\r\n"
                   "Content-Type: " + contentType + "\r\n"
                   "Content-Length: " + std::to_string(fileContent.length()) + "\r\n"
                   "\r\n" + fileContent;

        this->clientResponses.push_back(std::make_pair(clientSocket, response)); std::cerr << "Metodo de solicitud no valido." << std::endl; //TODO
    }
//		close(clientSocket);
}


void    Server::handleDeleteRequest(int clientSocket)
{
	std::cout << "----SOLICITUD DELETE-----\n";
	std::string deleted = getDeletedFilename();
	std::cout << "Deleted es: " << deleted << std::endl;
	if(remove(deleted.c_str()) != 0)
		std::cout << "El archivo no pudo ser eliminado\n";
	else
		std::cout << "Archivo eliminado correctamente\n";

	std::string staticContent = loadStatic();
	std::string resp = "HTTP/1.1 200 OK\r\n"
					   "Content-Type: text/html\r\n"
					   "Content-Length: " + std::to_string(staticContent.length()) + "\r\n"
					   "\r\n" + staticContent;
	this->clientResponses.push_back(std::make_pair(clientSocket, resp));
//	sendResponse(clientSocket, resp);
}

/*
void  Server::handleGetRequest(int clientSocket) 
{
    std::string response;
    std::string fileName = getRequestedFilename();
	std::cout << "Getted Filename es: " << fileName << std::endl;

    if (fileName != "index.html" && fileName != " /" && !fileName.empty()) 
	{
		std::string fileContent = loadStaticContent(fileName);
		if (!fileContent.empty()) 
		{
			response = "HTTP/1.1 200 OK\r\n"
					   "Content-Type: text/html\r\n"
					   "Content-Length: " + std::to_string(fileContent.length()) + "\r\n"
					   "\r\n" + fileContent;
		} 
		
		else 
		{
			response = "HTTP/1.1 404 Not Found\r\n"
                       "Content-Type: text/html\r\n"
                       "Content-Length: 0\r\n"
                       "\r\n";
		}
	} 
	
	else 
	{
		std::string staticContent = loadStatic();
		response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/html\r\n"
                   "Content-Length: " + std::to_string(staticContent.length()) + "\r\n"
                   "\r\n" + staticContent;
	}
	
	this->clientResponses.push_back(std::make_pair(clientSocket, response));

//	sendResponse(clientSocket, response);

}*/

std::string readFileContent(const std::string& filename) 
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

std::string getFileExt(const std::string &filename)
{
    size_t dotIndex = filename.find_last_of(".");

    if (dotIndex != std::string::npos && dotIndex < filename.size() - 1)
    {
        size_t questionMarkIndex = filename.find_last_of("?");
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

std::string getContentType(const std::string& extension) {
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

std::string generateDownloadResponse(const std::string& filename) 
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

std::string getCGIInterpreter(const std::string &ext) {
    std::string interpreter;
    if (ext == "sh") {
        interpreter = "/bin/sh";
    } else if (ext == "py") {
        interpreter = "/usr/bin/python3";
    } else if (ext == "pl") {
        //TODO route to perl
    } else if (ext == "php") {
        //TODO route to php
    } else if (ext == "rb") {
        //TODO route to ruby
    } else if (ext == "js" || ext == "ts") {
        //TODO route to node.js
    }

    return interpreter;
}

std::string executeCGI(std::string &executable, const std::string &ext) {
    std::string response;
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "Fatal error on pipe: could not execute CGI\n" << std::endl;
        return "error";
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Fatal error on fork: could not execute CGI\n" << std::endl;
        return "error";
    } else if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        std::string interpreter = getCGIInterpreter(ext);
        //char **argv = getArguments(); //TODO in case there are more arguments

        //TODO just for testing
        char *argv[] = {interpreter.substr(interpreter.find_last_of('/') + 1).data(), executable.data(), NULL};
        //TODO just for testing

        execve(interpreter.data(), argv, NULL);
        std::cerr << "Error: " << executable.substr(executable.find_last_of('/') + 1)
        << ": could not execute CGI.\n";
        write(1, "error", sizeof("error"));
        exit(1);
    } else {
        int status;
        char buffer[1024];
        ssize_t bytesRead = 0;

        close(pipefd[1]);
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) > 0))) {
            buffer[bytesRead] = '\0';
            response.append(buffer);
        }
        close(pipefd[0]);
        waitpid(pid, &status, 0);
        std::cout << "response from the CGI: " << response << "\n"; //TODO
    }
    return response;
}

void    Server::assignCGIResponse(std::string &response) {
    std::pair<std::string, std::string> content;
    std::string contentType = content.first;
    std::string fileContent = content.second;

    if (response == "error") {
        content = loadStaticContent("500.html");
        contentType = content.first;
        fileContent = content.second;
        response = "HTTP/1.1 500 Internal Server Error\r\n"
                   "Content-Type: " + contentType + "\r\nContent-Length: "
                   + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
    } else if (response == "not found" ) {
        content = loadStaticContent("404.html");
        contentType = content.first;
        fileContent = content.second;
        response = "HTTP/1.1 404 Not founde\r\n"
                   "Content-Type: " + contentType + "\r\nContent-Length: "
                   + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
    } else if (response == "forbidden") {
            //TODO differenciate between 404 and 403 (if you search for some non existing file with an extension, it returns 403)
            content = loadStaticContent("403.html");
            contentType = content.first;
            fileContent = content.second;
            response = "HTTP/1.1 403 Forbidden\r\n"
                       "Content-Type: " + contentType + "\r\nContent-Length: "
                       + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
    } else {
        response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/plain\r\n"
                   "Content-Length: " + std::to_string(response.length()) + "\r\n\r\n"
                   + response;
    }
}

void    showAutoIndex(std::string &fileName, std::string &response) {
    if (fileName == "" || fileName == "favicon.ico")
        fileName = ".";
    if(fileName.back() == '/' || fileName == ".")
    {
        std::cout << "DIRECTORIO\n\n";
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
                std::string path = fileName + "/" + name;

                if(entry->d_type == DT_DIR)
                {
                    directories.push_back(name);
                }

                else if(entry->d_type == DT_REG)
                {
                    files.push_back(name);
                }
            }
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

        closedir(dir);
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

void Server::handleGetRequest(int clientSocket, ServerConf &conf)
{
	std::string response;
	std::string fileName = getRequestedFilename();
    std::string ext = getFileExt(fileName);
    std::pair<std::string, std::string> content = loadStaticContent(fileName);
    std::string contentType = content.first;
    std::string fileContent = content.second;

    std::cout << "Getted Filename: " << fileName << std::endl;
    if (ext.empty() || (ext != "sh" && ext != "py" && ext != "pl"
                        && ext != "php" && ext != "rb" && ext != "js")) {
        DIR *dir;
        if (fileName.empty()) {
            dir = opendir(".");
        } else {
            dir = opendir(fileName.c_str());
        }

        if (dir == NULL) {
            if (!fileContent.empty()) {
                response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: " + contentType + "\r\nContent-Length: "
                           + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
            } else {
                content = loadStaticContent("404.html");
                contentType = content.first;
                fileContent = content.second;
                response = "HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: text/html\r\nContent-Length: "
                           + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
            }
        } else {
            bool isIndex = false;
            struct dirent* entry;
            ServerConf::LocationIterator it = conf.findLocation(fileName);

            while ((entry = readdir(dir)) != NULL) {
                std::string name = entry->d_name;
                if (name == "." || name == "..") {
                    continue;
                } else if (name == "index.html") {
                    isIndex = true;
                }

                if (it != conf.locationEnd() && !it->lo_indexes.empty()) {
                    for (std::vector<std::string>::iterator it2 = it->lo_indexes.begin();
                         it2 != it->lo_indexes.end(); it2++) {
                        if (name == *it2 || (fileName + name) == *it2) {
                            content = loadStaticContent(fileName + name);
                            contentType = content.first;
                            fileContent = content.second;
                            response = "HTTP/1.1 200 OK\r\n"
                                       "Content-Type: " + contentType + "\r\nContent-Length: "
                                       + std::to_string(fileContent.length()) + "\r\n\r\n" += fileContent;
                            break ;
                        }
                    }
                }
                if (!response.empty()) {
                    break ;
                }
            }

            if (response.empty()) {
                if (isIndex) {
                    content = loadStaticContent(fileName + "index.html");
                    fileContent = content.second;
                    response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\nContent-Length: "
                               + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
                } else if (it != conf.locationEnd() && it->autoIndex) {
                    showAutoIndex(fileName, response);
                } else {
                    content = loadStaticContent("404.html");
                    contentType = content.first;
                    fileContent = content.second;
                    response = "HTTP/1.1 404 Not Found\r\n"
                               "Content-Type: text/html\r\nContent-Length: "
                               + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
                }
            }
        }
        closedir(dir);
    } else {
        std::string filePath = fileName.substr(0, fileName.find_last_of('/') + 1);
        ServerConf::LocationIterator location = conf.findLocation(filePath);

        ext.erase(0, 0); //TODO didnt want to change getFileExt just in case anything uses it
        if (location != conf.locationEnd()) {
            if (std::find(location->cgiLangs.begin(), location->cgiLangs.end(), ext) != location->cgiLangs.end()) {
                response = executeCGI(fileName, ext);
            } else {
                content = loadStaticContent(fileName);
                fileContent = content.second;
                if (fileContent.empty()) {
                    response = "not found";
                } else {
                    response = "forbidden";
                }
            }
            assignCGIResponse(response);
        } else {
            location = conf.findLocation("/" + fileName);
            if (location != conf.locationEnd()) {
                if (std::find(location->cgiLangs.begin(), location->cgiLangs.end(), ext) != location->cgiLangs.end()) {
                    response = executeCGI(fileName, ext);
                } else {
                    content = loadStaticContent(fileName);
                    fileContent = content.second;
                    if (fileContent.empty()) {
                        response = "not found";
                    } else {
                        response = "forbidden";
                    }
                }
                assignCGIResponse(response);
            } else {
                std::string size = readFileContent(fileName);

                if (!size.empty()) {
                    content = loadStaticContent("403.html");
                    contentType = content.first;
                    fileContent = content.second;
                    response = "HTTP/1.1 403 Forbidden\r\n"
                               "Content-Type: " + contentType + "\r\nContent-Length: "
                               + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
                } else {
                    content = loadStaticContent("404.html");
                    contentType = content.first;
                    fileContent = content.second;
                    response = "HTTP/1.1 404 Not found\r\n"
                               "Content-Type: " + contentType + "\r\nContent-Length: "
                               + std::to_string(fileContent.length()) + "\r\n\r\n" + fileContent;
                }
            }
        }
    }

    this->clientResponses.push_back(std::make_pair(clientSocket, response));
    // sendResponse(clientSocket, response);
}


void Server::handlePostRequest(int clientSocket) 
{
	std::string boundary = extractBoundary();
	std::cout << "Valor de boundary: " << boundary << std::endl;
	
	if (!boundary.empty()) 
	{
		saveFileContent(boundary);
	}
	
	std::string staticContent = loadStatic();
	std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: " + std::to_string(staticContent.length()) + "\r\n"
                           "\r\n" + staticContent;
	
	this->clientResponses.push_back(std::make_pair(clientSocket, response));
//	sendResponse(clientSocket, response);
}


void Server::extractValues(const std::string &key, std::string& value) 
{
	MapIterator it = this->keyValuePairs.find(key);
	if (it != this->keyValuePairs.end()) 
	{
		const std::list<std::string>& values = it->second;
		if (!values.empty()) 
		{
			value = values.front();
			std::cout << key << ": " << value << std::endl;
		} 
		
		else 
		{
			std::cout << "No value found for " << key << std::endl;
		}
	} 
	else 
	{
		std::cout << "Key not found: " << key << std::endl;
	}
}

void Server::extractFilename(void) 
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


void Server::parseRequest(void)
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
