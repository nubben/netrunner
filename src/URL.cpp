#include "URL.h"

#include "StringUtils.h"

/*
 * From the following RFC: https://tools.ietf.org/html/rfc3986
 *
 * pct-encoded = "%" HEXDIG HEXDIG
 * reserved    = gen-delims / sub-delims
 * gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@"
 * sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
 *                / "*" / "+" / "," / ";" / "="
 *
 */

enum URIParseState {
    SCHEME,
    FIRST_SLASH,
    SECOND_SLASH_OR_ELSE,
    AUTHORITY,
    AUTHORITY_USERINFO, /* The part before '@' */
    AUTHORITY_PASSWORD, /* RFC states that we should either reject or ignore it (We ignore it) */
    AUTHORITY_HOST,
    AUTHORITY_PORT,
    PATH,
    QUERY,
    FRAGMENT,
};

// TODO
bool isValidCharacter(char c);
bool isValidCharacter(char c) {
    return true;
}

// put inside URL(std::string) constructor?
std::tuple<std::unique_ptr<URL>,enum URIParseError> parseUri(std::string raw) {
    std::unique_ptr<URL> uri = std::make_unique<URL>();
    uri->path = "/";
    unsigned int cursor = 0;
    unsigned int last = 0;
    unsigned int lastSemicolon = 0;
    bool isPercentEncoded = true;
    enum URIParseState state = SCHEME;
    // TODO Validate at the end that every field were defined (ie: http:// is valid, but it's clearly not)
    // Remember file:// doesn't need a port (for end validation)

    if (raw[cursor] == '/') {
        // unless it's / for relative URLs
        // need this so we can merge URLs
        
        // make sure it's not // (relative scheme)
        if (raw.length() > 1) {
            if (raw[cursor + 1] == '/') {
                // there is a host
                uri->scheme = "relative";
                state = AUTHORITY;
            } else {
                 // relative path
                uri->scheme = ""; // probably doesn't matter if it's "" or "relative"
                state = PATH;
            }
        }
    } else {
        // First character of scheme MUST be alphabetic
        if (!isalpha(raw[cursor])) {
            std::cerr << "invalid scheme: '" << raw.substr(last, cursor - last+1) << "'" << std::endl;
            return std::make_tuple(std::move(uri), URI_PARSE_ERROR_SCHEME);
        }
    }
    for (cursor = 1; cursor < raw.length(); cursor++) {
        /* TODO
         * Allow scheme-less uri (and fallback to https/http) */
        if (state == SCHEME) {
            if (raw[cursor] == ':') {
                uri->scheme = toLowercase(raw.substr(0, cursor));
                /* TODO
                 * Put default port now (Should use a table for that but
                 * I don't know C++ enough) */
                if (uri->scheme == "http") {
                    uri->port = 80;
                }
                state = FIRST_SLASH;
            } else if (!isalpha(raw[cursor]) && !isdigit(raw[cursor]) && raw[cursor] != '+' &&
                       raw[cursor] != '-' && raw[cursor] != '.'  && raw[cursor] != '/') { // why this exception list?
                /* URI MUST have a scheme  */
                std::cerr << "invalid scheme: '" << raw.substr(last, cursor - last+1) << "' cursor: " << cursor << std::endl;
                return std::make_tuple(std::move(uri), URI_PARSE_ERROR_SCHEME);
            }
        } else if (state == FIRST_SLASH) {
            if (raw[cursor] == '/') {
                state = SECOND_SLASH_OR_ELSE;
            } else {
                std::cerr << "missing '/' after scheme" << std::endl;
                return std::make_tuple(std::move(uri), URI_PARSE_ERROR_SCHEME);
            }
        } else if (state == SECOND_SLASH_OR_ELSE) {
            if (raw[cursor] == '/') {
                last = cursor + 1;
                state = AUTHORITY;
                if (uri->scheme == "file") {
                    std::cout << "file scheme, current path[" << uri->path << "]" << std::endl;
                    state = PATH;
                }
            } else {
                // TODO Handle this, URI may have only one slash
                std::cerr << "URI with only one '/' not currently supported" << std::endl;
                return std::make_tuple(std::move(uri), URI_PARSE_ERROR_SCHEME);
            }
        } else if (state == AUTHORITY) {
            /* At this point, this could either be the semi colon for
             * the password or for the port*/
            if (raw[cursor] == ':') {
                lastSemicolon = cursor;
            } else if (raw[cursor] == '@') {
                uri->userinfo = raw.substr(last, cursor - last);
                last = cursor + 1;
                state = AUTHORITY_HOST;
                // Authority is finished, everything should be considered as the host[port].
                //  TODO terminated by the next slash ("/"), question mark ("?"), or number sign ("#") character, or by the end of the URI.
                //  What to do when ? and # ?
            } else if (raw[cursor] == '/') {
                if (lastSemicolon > 0) {
                    // TODO Validate port
                    if (cursor - lastSemicolon - 1 > 0) {
                        uri->port = std::stoi(raw.substr(lastSemicolon+1, cursor - lastSemicolon+1));
                    }
                    uri->host = raw.substr(last, lastSemicolon - last);
                } else {
                    uri->host = raw.substr(last, cursor - last);
                }
                last = cursor;
                cursor--;
                state = PATH;
            } else if (raw[cursor] == '?' || raw[cursor] == '#') {
                uri->host = raw.substr(last, cursor - last);
                last = cursor;
                if (raw[cursor] == '?') {
                    state = QUERY;
                } else {
                    state = FRAGMENT;
                }
            } else if (cursor + 1 == raw.length()) {
                uri->host = raw.substr(last, lastSemicolon - last);
                uri->path = "/";
                break;
            } else {
                if (isPercentEncoded && !isValidCharacter(raw[cursor])) {
                    isPercentEncoded = false;
                }
            }
        // TODO Accept Ipv weirdness (ipversion and literal)
        /* We are accepting pretty much anything here.. However not everything
         * is valid. Should we try to validate at this point ? */
        } else if (state == AUTHORITY_HOST) {
            if (raw[cursor] == ':') {
                uri->host = raw.substr(last, cursor - last);
                last = cursor+1;
                state = AUTHORITY_PORT;
            } else if (raw[cursor] == '/') {
                uri->host = raw.substr(last, cursor - last);
                last = cursor;
                cursor--;
                state = PATH;
            } else {
                if (isPercentEncoded && !isValidCharacter(raw[cursor])) {
                    isPercentEncoded = false;
                }
            }
        } else if (state == AUTHORITY_PORT) {
            if (raw[cursor] == '/') {
                if (cursor - last > 0) {
                    /* RFC doesn't specify the max port number, so at this point
                     * just accept it. */
                    uri->port = std::stoi(raw.substr(last, cursor - last));
                }
                last = cursor;
                cursor--;
                state = PATH;
            } else if (!isdigit(raw[cursor])) {
                std::cerr << "invalid port: '" << raw.substr(last, cursor - last + 1) << "'" << std::endl;
                return std::make_tuple(std::move(uri), URI_PARSE_ERROR_PORT);
            }
        } else if (state == PATH) {
            if (raw[cursor] == '?' || raw[cursor] == '#') {
                uri->path = raw.substr(last, cursor - last);
                last = cursor;
                if (raw[cursor] == '?') {
                    state = QUERY;
                } else {
                    state = FRAGMENT;
                }
            } else if (cursor + 1 == raw.length()) {
                uri->path = raw.substr(last, cursor + 1 - last);
                break;
            }
        } else if (state == QUERY) {
            if (raw[cursor] == '#') {
                uri->query = raw.substr(last + 1, cursor - last - 1);
                last = cursor;
                state = FRAGMENT;
            } else if (cursor + 1 == raw.length()) {
                uri->query = raw.substr(last + 1, cursor + 1 - last);
                break;
            }
        } else if (state == FRAGMENT) {
            if (cursor + 1 == raw.length()) {
                uri->fragment = raw.substr(last + 1, cursor + 1 - last);
                break;
            }
        }
    }
    return std::make_tuple(std::move(uri), URI_PARSE_ERROR_NONE);
}


URL::URL() {
    scheme = "";
    userinfo = "";
    host = "";
    port = 0;
    path = "";
    query = "";
    fragment = "";
}

URL::URL(std::string const& url) {
    construct(url);
}

std::string URL::toString() const {
    //std::cout << "scheme[" << scheme << "] host[" << host << "] path [" << path << "]" << std::endl;
    if (isRelative()) {
        return path;
    }
    return scheme + "://" + host + path;
}

bool URL::isRelative() const {
    return scheme.size() == 0;
}

URL URL::merge(URL const& url) const {
    if (!url.isRelative()) {
        return url.copy();
    }
    
    URL returnURL = copy();

    if (url.path[0] == '/' && url.path[1] == '/') {
        auto slashPos = url.path.find('/', 2);
        returnURL.host = url.path.substr(2, slashPos - 2);
        if (slashPos == std::string::npos) {
            returnURL.path = "/";
        } else {
            returnURL.path = url.path.substr(slashPos);
        }
    } else if (url.path[0] == '/') {
        returnURL.path = url.path;
    } else {
        if (returnURL.path.back() != '/') {
            auto finalSlashPos = returnURL.path.find_last_of('/');
            returnURL.path.erase(finalSlashPos + 1);
        }
        returnURL.path += url.path;
    }
    
    /*
    auto hashPos = returnURL.document.find("#");
    if (hashPos != std::string::npos) {
        returnURL.document = returnURL.document.substr(0, hashPos);
    }
    */
    
    return returnURL;
}

void URL::construct(std::string const& url) {
    auto result = parseUri(url);
    if (std::get<1>(result) != URI_PARSE_ERROR_NONE) {
        // TODO We probably wanna handle this better..
        std::cerr << "error parsing uri" << std::endl;
        return;
    }
    std::unique_ptr<URL> uri = std::move(std::get<0>(result));
    scheme = uri->scheme;
    userinfo = uri->userinfo;
    host = uri->host;
    port = uri->port;
    path = uri->path;
    query = uri->query;
    fragment = uri->fragment;
    
    /*
    scheme = getSchemeFromURL(url);
    if (scheme.size() != 0) {
        host = getHostFromURL(url);
        path = getDocumentFromURL(url);
    } else {
        host = "";
        path = url;
    }

    if (path.size() == 0) {
        path = "/";
    }
    */
}

URL URL::copy() const {
    URL url;
    url.scheme = scheme;
    url.userinfo = userinfo;
    url.host = host;
    url.port = port;
    url.path = path;
    url.query = query;
    url.fragment = fragment;
    return url;
}

std::ostream& operator<<(std::ostream& out, URL const& url) {
    out << url.toString();
    return out;
}
