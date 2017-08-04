#ifndef URL_H
#define URL_H

#include <string>
#include <memory>
#include <iostream>

struct Authority {
    std::string userinfo;
    std::string host;
    int port;
};

struct URL {
    URL();
    URL(std::string const& url);

    std::string toString() const;
    bool isRelative() const;
    URL merge(URL const& url) const;
    
    // switching to the official RFC names
    std::string scheme; // was protocol
    //struct Authority authority; /* Can be empty */
    std::string userinfo;
    std::string host;
    int port;
    std::string path; // was document
    std::string query; // ?blablabla=asdfasf....
    std::string fragment; // #asfawefm
    
private:
    void construct(std::string const& url);
    URL copy() const;
};

std::unique_ptr<URL> parseUri(std::string raw);
std::ostream& operator<<(std::ostream& out, URL const& url);

#endif
