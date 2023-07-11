#include "server.hpp"
#include <fcntl.h>

std::string cut(const std::string& cadena, const std::string& separador) 
{
    std::size_t pos = cadena.find(separador);
    if (pos != std::string::npos) 
    {
        return cadena.substr(0, pos);
    }
    return cadena;
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

Server::Server(int port) : port(port), serverSocket(0)
{
}

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



void Server::start(SocketManager &serverSockets)
{
    int serverSocket = serverSockets.listenOnSock(serverSockets.sockBegin());
	fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	fd_set readfds;
	fd_set writefds;
	
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_SET(serverSocket, &readfds);
	
	int biggest = serverSocket;
	
	std::vector<int> clientSocket;
	int result = 0;
	while (true)
    {
	   
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
//				fcntl(newClient, F_SETFL, O_NONBLOCK);

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
				break;
				
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
				break;
			}
			else
				++it;
				
			
		}

	}
	
	close(serverSocket);
}
/*
void Server::start(SocketManager &serverSockets)
{
    int serverSocket = serverSockets.listenOnSock(serverSockets.sockBegin());
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
    fd_set readfds;
    fd_set writefds;

    int biggest = serverSocket;

    std::vector<int> clientSocket;
    int result = 0;
    int requestCount = 0;
    int childProcessCount = 0;

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
        if (FD_ISSET(serverSocket, &readfds))
        {
            int newClient = acceptClientConnection(serverSocket);
            if (newClient != -1)
            {
                clientSocket.push_back(newClient);
                FD_SET(clientSocket.back(), &readfds);
                if (clientSocket.back() > biggest)
                {
                    biggest = clientSocket.back();
                }

                // Increment the request count
                requestCount++;

                // Check if we reached the limit for the current process
                if (requestCount == 16000)
                {
                    // Fork a new process
                    pid_t pid = fork();
                    if (pid < 0)
                    {
                        // Forking failed, handle the error
                        perror("Fork failed");
                        exit(1);
                    }
                    else if (pid == 0)
                    {
                        // Child process
                        // Reset the request count and close unnecessary sockets
                        requestCount = 0;
                        close(serverSocket);
                        for (int socket : clientSocket)
                        {
                            close(socket);
                        }
                        clientSocket.clear();

                        // Break out of the loop to start processing requests in the child process
                        break;
                    }
                    else
                    {
                        // Parent process
                        // Close unnecessary sockets and clear the client socket vector
                        close(newClient);
                        clientSocket.clear();
                        childProcessCount++;

                        // Check if we reached the limit for the parent process
                        if (childProcessCount == 16000)
                        {
                            // Wait for the child process to finish
                            wait(NULL);
                            childProcessCount = 0;
                        }
                    }
                }
            }
        }
        for (std::vector<int>::iterator it = clientSocket.begin(); it != clientSocket.end();)
        {
            if (FD_ISSET(*it, &readfds))
            {
                handleClientRequest(*it);
                FD_CLR(*it, &readfds);
                FD_SET(*it, &writefds);

                it = clientSocket.erase(it);
            }
        }

        for (std::vector<Pair>::iterator it = this->clientResponses.begin(); it != this->clientResponses.end();)
        {
            if (FD_ISSET(it->first, &writefds))
            {
                int fd = it->first;
                std::string re = it->second;
                sendResponse(fd, re);
                close(fd);
                it = this->clientResponses.erase(it);
                FD_CLR(it->first, &writefds);
            }
        }
    }

    close(serverSocket);
}*/


int Server::createServerSocket() 
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) 
	{
		std::cerr << "Error al crear el socket" << std::endl;
		exit(1);
	}
	return serverSocket;
}

void Server::bindServerSocket(int serverSocket, int port) 
{
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	
	if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) 
	{
		std::cerr << "Error al vincular el socket" << std::endl;
		close(serverSocket);
		exit(1);
	}
}

int Server::acceptClientConnection(int serverSocket) 
{
	int clientSocket = accept(serverSocket, nullptr, nullptr);
	if (clientSocket < 0) 
	{
		if(errno == EWOULDBLOCK || errno == EAGAIN)
			return(-1);
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

void Server::sendResponse(int clientSocket, const std::string& response) 
{
	ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
	if (bytesSent != response.length()) 
	{
		std::cerr << "Error al enviar la respuesta al cliente." << std::endl;
	}
}

void Server::handleClientRequest(int clientSocket) 
{

//	char c;
	char buffer[RECV_SIZE];
	ssize_t bytesRead;
	int contentLength = 0;
	bool foundEndOfHeaders = false;
	std::string endOfHeaders = "\r\n\r\n";
	std::string contentLengthHeader = "Content-Length: ";
	
//	while ((bytesRead = recv(clientSocket, &c, 1, 0)) > 0) 
	while ((bytesRead = recv(clientSocket, buffer, RECV_SIZE - 1, 0)) > 0)
	{
		this->data.append(buffer, bytesRead);
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
	std::cout << this->data << std::endl;
	
	
	parseRequest();
	printValueForKey("Content-Type");
	extractFilename();
	printMap();
	crossRoads(clientSocket);
	clearAll();
}

void Server::clearAll(void)
{
	this->fileName.clear();
	this->data.clear();
	this->keyValuePairs.clear();
}

void Server::crossRoads(int clientSocket)
{
	std::string requestMethod = cut(this->data, " ");
	
	
	std::cout << "REQUEST METHOD: " << requestMethod << std::endl;
	
	
		if(requestMethod == "GET")
		{
			handleGetRequest(clientSocket);
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
			std::cerr << "Metodo de solicitud no valido." << std::endl;
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
					   "Content-Type: image/jpeg\r\n"
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












