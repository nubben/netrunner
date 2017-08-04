#include "HTTPRequest.h"
#include <errno.h>
#include <iostream>
#include <string.h>


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#define ssize_t intptr_t
#else
// unix includes here
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

#ifdef WIN32
// Initialize Winsock
WSADATA wsaData;
int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

HTTPRequest::HTTPRequest(const std::shared_ptr<URL> u) {
#ifdef WIN32
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return;
	}
#endif
    uri = u;
    version = Version::HTTP10;
    method = Method::GET;
    userAgent = "NetRunner";
}

bool HTTPRequest::sendRequest(std::function<void(const HTTPResponse&)> responseCallback) const {
    struct addrinfo hints;
    struct addrinfo *serverInfo = nullptr;
    std::string host = uri->host;
    std::string document = uri->path;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    const int res = getaddrinfo(uri->host.c_str(), std::to_string(uri->port).c_str(), &hints, &serverInfo);
    if (res != 0) {
        std::cout << "Could not lookup " << host << ": " << res << std::endl;
        freeaddrinfo(serverInfo);
        return false;
    }

    const int sock = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (sock == -1) {
        std::cout << "Could not create socket: " << errno << std::endl;
        freeaddrinfo(serverInfo);
        return false;
    }

    if (connect(sock, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1) {
        std::cout << "Could not connect to: " << host << ":" << errno << std::endl;
        freeaddrinfo(serverInfo);
        return false;
    }

    const std::string request = methodToString(method) + std::string(" ") + document + std::string(" ") + versionToString(version) + std::string("\r\nHost: ") + host + std::string("\r\nUser-Agent: ") + userAgent + std::string("\r\n\r\n");
    const ssize_t sent = send(sock, request.c_str(), request.length(), 0);
    if (sent == -1) {
        std::cout << "Could not send \"" << request << "\": " << errno << std::endl;
        freeaddrinfo(serverInfo);
        return false;
    }

    std::string response;
    char buffer[512];
    ssize_t received;
    while ((received = recv(sock, buffer, sizeof(buffer), 0)) != 0) {
        response += std::string(buffer, static_cast<unsigned int>(received));
    }

    responseCallback(HTTPResponse(response));

    freeaddrinfo(serverInfo);
    return true;
}

const std::string HTTPRequest::versionToString(const Version ver) const {
    switch (ver) {
        case Version::HTTP10:
            return "HTTP/1.0";
        default:
            return "ERROR";
    }
}

const std::string HTTPRequest::methodToString(const Method meth) const {
    switch (meth) {
        case Method::GET:
            return "GET";
        case Method::POST:
            return "POST";
        default:
            return "ERROR";
    }
}
