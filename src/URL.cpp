#include "URL.h"

#include "StringUtils.h"

URL::URL() {
    protocol = "";
    host = "";
    document = "";
}

URL::URL(std::string const& url) {
    construct(url);
}

std::string URL::toString() const {
    if (isRelative()) {
        return document;
    }
    return protocol + "://" + host + document;
}

bool URL::isRelative() const {
    return protocol.size() == 0;
}

URL URL::merge(URL const& url) const {
    if (!url.isRelative()) {
        return url.copy();
    }
    
    URL returnURL = copy();

    if (url.document[0] == '/' && url.document[1] == '/') {
        auto slashPos = url.document.find('/', 2);
        returnURL.host = url.document.substr(2, slashPos - 2);
        if (slashPos == std::string::npos) {
            returnURL.document = "/";
        } else {
            returnURL.document = url.document.substr(slashPos);
        }
    } else if (url.document[0] == '/') {
        returnURL.document = url.document;
    } else {
        if (returnURL.document.back() != '/') {
            auto finalSlashPos = returnURL.document.find_last_of('/');
            returnURL.document.erase(finalSlashPos + 1);
        }
        returnURL.document += url.document;
    }
    
    return returnURL;
}

void URL::construct(std::string const& url) {
    protocol = getProtocolFromURL(url);
    if (protocol.size() != 0) {
        host = getHostFromURL(url);
        document = getDocumentFromURL(url);
    } else {
        host = "";
        document = url;
    }

    if (document.size() == 0) {
        document = "/";
    }

}

URL URL::copy() const {
    URL url;
    url.protocol = protocol;
    url.host = host;
    url.document = document;
    return url;
}

std::ostream& operator<<(std::ostream& out, URL const& url) {
    out << url.toString();
    return out;
}
