#include "URL.h"

#include "StringUtils.h"

URL::URL(std::string const& url) {
    construct(url);
}

std::string URL::toString() {
    if (isRelative()) {
        return document;
    }
    return protocol + "://" + host + document;
}

bool URL::isRelative() {
    return protocol.size() == 0;
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

std::ostream& operator<<(std::ostream& out, URL& url) {
    out << url.toString();
    return out;
}
