#ifndef URL_H
#define URL_H

#include <string>
#include <iostream>

struct URL {
    URL(std::string const& url);

    std::string toString();
    bool isRelative();
    
    std::string protocol;
    std::string host;
    std::string document;

private:
    void construct(std::string const& url);
};

std::ostream& operator<<(std::ostream& out, URL& url);

#endif
