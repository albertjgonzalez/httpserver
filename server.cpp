#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>
#include <string>

int runserver()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		return 1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		WSACleanup();
		return 1;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(8080);
	if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	char buffer[1024];
	int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (bytesReceived > 0) {
		buffer[bytesReceived] = '\0';
	}

	std::string request(buffer);
	std::string method;
	std::string path;

	size_t firstSpace = request.find(' ');
	if (firstSpace != std::string::npos) {
		method = request.substr(0, firstSpace);
		size_t secondSpace = request.find(' ', firstSpace + 1);
		if (secondSpace != std::string::npos) {
			path = request.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		}
	}

	const char* response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 19\r\n"
		"\r\n"
		"Hello from server!\n";

	send(clientSocket, response, strlen(response), 0);

	closesocket(clientSocket);
	closesocket(listenSocket);
	WSACleanup();

	return 0;
}