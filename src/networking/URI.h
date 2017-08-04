#ifndef URI_H
#define URI_H

#include <memory>
#include <string>

struct Authority {
    std::string userinfo;
    std::string host;
    int port;
};

struct URI {
    std::string scheme;
    struct Authority authority; /* Can be empty */
    std::string path;
    std::string query; // ?blablabla=asdfasf....
    std::string fragment; // #asfawefm
};

std::unique_ptr<URI> parseUri(std::string raw);

#endif
