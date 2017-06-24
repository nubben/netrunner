#include "HTTPRequest.h"
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

HTTPRequest::HTTPRequest(const std::string &host, const std::string &document) {
    this->document = document;
    this->version = Version::HTTP10;
    this->method = Method::GET;
    this->host = host;
    this->userAgent = "NetRunner";
}

const bool HTTPRequest::sendRequest(std::function<void(const HTTPResponse&)> responseCallback) const {
    struct addrinfo hints;
    struct addrinfo *serverInfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    const int res = getaddrinfo(host.c_str(), "80", &hints, &serverInfo);
    if (res != 0) {
        std::cout << "Could not lookup " << host << ": " << res << std::endl;
        return false;
    }

    const int sock = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (sock == -1) {
        std::cout << "Could not create socket: " << errno << std::endl;
        freeaddrinfo(serverInfo);
        return false;
    }

    if (connect(sock, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1) {
        std::cout << "Could not connect: " << errno << std::endl;
        freeaddrinfo(serverInfo);
        return false;
    }

    const std::string request = methodToString(method) + std::string(" ") + document + std::string(" ") + versionToString(version) + std::string("\r\nHost: ") + host + std::string("\r\nUser-Agent: ") + userAgent + std::string("\r\n\r\n");
    const size_t sent = send(sock, request.c_str(), request.length(), 0);
    if (sent == -1) {
        std::cout << "Could not send \"" << request << "\": " << errno << std::endl;
        freeaddrinfo(serverInfo);
        return false;
    }

    std::string response;
    char buffer[512];
    size_t received;
    while ((received = recv(sock, buffer, sizeof(buffer), 0)) != 0) {
        response += std::string(buffer, received);
    }

    responseCallback(HTTPResponse(response));

    freeaddrinfo(serverInfo);
    return true;
}

const std::string HTTPRequest::versionToString(const Version version) const {
    switch (version) {
        case Version::HTTP10:
            return "HTTP/1.0";
    }
    return "ERROR";
}

const std::string HTTPRequest::methodToString(const Method method) const {
    switch (method) {
        case Method::GET:
            return "GET";
        case Method::POST:
            return "POST";
    }
    return "ERROR";
}
