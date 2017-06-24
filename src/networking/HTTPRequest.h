#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "HTTPResponse.h"
#include <functional>
#include <string>

enum class Version {
    HTTP10
};

enum class Method {
    GET,
    POST
};

class HTTPRequest {
public:
    HTTPRequest(std::string host, std::string document);
    bool sendRequest(std::function<void(HTTPResponse)> responseCallback);
    std::string versionToString(Version version);
    std::string methodToString(Method method);
private:
    std::string document;
    Version version;
    Method method;
    std::string host;
    std::string userAgent;
};

#endif
