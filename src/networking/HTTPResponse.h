#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <map>
#include <string>

class HTTPResponse {
public:
    HTTPResponse(std::string rawRequest);
    std::string version;
    std::string status;
    std::map<std::string, std::string> properties;
    std::string body;
    int statusCode;
private:
};

#endif
