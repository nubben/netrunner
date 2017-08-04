#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "HTTPResponse.h"
#include "URI.h"

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
    HTTPRequest(const std::shared_ptr<URI> u, const std::string &doc);
    bool sendRequest(std::function<void(const HTTPResponse&)> responseCallback) const;
    const std::string versionToString(const Version version) const;
    const std::string methodToString(const Method method) const;
private:
    std::string document;
    Version version;
    Method method;
    std::string userAgent;
    std::shared_ptr<URI> uri;
};

#endif
