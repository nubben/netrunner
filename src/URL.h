#ifndef URL_H
#define URL_H

#include <string>
#include <iostream>

// I'm really mixed about this being a struct like this
// probably fine for now
struct URL {
    URL();
    URL(std::string const& url);

    std::string toString() const;
    bool isRelative() const;
    URL merge(URL const& url) const;
    
    std::string protocol;
    std::string host;
    std::string document;

private:
    void construct(std::string const& url);
    URL copy() const;
};

std::ostream& operator<<(std::ostream& out, URL const& url);

#endif
