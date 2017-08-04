#include "../StringUtils.h"
#include "URI.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

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

enum uri_parse_state {
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

std::unique_ptr<URI> parseUri(std::string raw) {
    std::unique_ptr<URI> uri = std::make_unique<URI>();
    unsigned int cursor = 0;
    unsigned int last = 0;
    unsigned int last_semicolon = 0;
    enum uri_parse_state state = SCHEME;
    // First character of scheme MUST be alphabetic
    if (!isalpha(raw[cursor])) {
        std::cout << "parse scheme error" << std::endl;
        return NULL;
    }
    for (cursor = 1; cursor < raw.length(); cursor++) {
        /* TODO
         * Allow scheme-less uri (and fallback to https/http) */
        if (state == SCHEME) {
            if (raw[cursor] == ':') {
                uri->scheme = stringToLower(raw.substr(0, cursor));
                /* TODO
                 * Put default port now (Should use a table for that but
                 * I don't know C++ enough) */
                if (uri->scheme == "http") {
                   uri->authority.port = 80; 
                }
                state = FIRST_SLASH;
            } else if (!isalpha(raw[cursor]) && !isdigit(raw[cursor]) && raw[cursor] != '+' &&
                    raw[cursor] != '-' && raw[cursor] != '.') {
                std::cout << "parse scheme error" << std::endl;
                return NULL;
            }
        } else if (state == FIRST_SLASH) {
            if (raw[cursor] == '/') {
                state = SECOND_SLASH_OR_ELSE;
            } else {
                std::cout << "parse error" << std::endl;
            }
        } else if (state == SECOND_SLASH_OR_ELSE) {
            if (raw[cursor] == '/') {
                last = cursor + 1;
                state = AUTHORITY;
            } else {
                // TODO Handle this, URI can have only one slash
            }
        } else if (state == AUTHORITY) {
            /* At this point, this could either be the semi colon for
             * the password or for the port*/
            if (raw[cursor] == ':') {
                last_semicolon = cursor;
            } else if (raw[cursor] == '@') {
                uri->authority.userinfo = raw.substr(last, cursor - last);
                last = cursor + 1;
                state = AUTHORITY_HOST;
            // Authority is finished, everything should be considered as the host[port].
            //  TODO terminated by the next slash ("/"), question mark ("?"), or number sign ("#") character, or by the end of the URI.
            //  What to do when ? and # ?
            } else if (raw[cursor] == '/') {
                if (last_semicolon > 0) {
                    // TODO Validate port
                    if (cursor - last_semicolon - 1 > 0) {
                        uri->authority.port = std::stoi(raw.substr(last_semicolon+1, cursor - last_semicolon+1));
                    }
                    uri->authority.host = raw.substr(last, last_semicolon - last);
                } else {
                    uri->authority.host = raw.substr(last, cursor - last);
                }
                last = cursor;
                cursor--;
                state = PATH;
            } else if (cursor + 1 == raw.length()) {
                uri->authority.host = raw.substr(last, last_semicolon - last);
                uri->path = "/";
                break;
            }
        } else if (state == AUTHORITY_HOST) {
            if (raw[cursor] == ':') {
                uri->authority.host = raw.substr(last, cursor - last);
                last = cursor+1;
                state = AUTHORITY_PORT;
            } else if (raw[cursor] == '/') {
                uri->authority.host = raw.substr(last, cursor - last);
                last = cursor;
                cursor--;
                state = PATH;
            }
        } else if (state == AUTHORITY_PORT) {
            if (raw[cursor] == '/') {
                if (cursor - last > 0) {
                    uri->authority.port = std::stoi(raw.substr(last, cursor - last));
                }
                last = cursor;
                cursor--;
                state = PATH;
            } else if (!isdigit(raw[cursor])) {

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
            if (raw[cursor] == '#' || cursor + 1 == raw.length()) {
                uri->query = raw.substr(last + 1, cursor + 1 - last);
                last = cursor;
            }
        } else if (state == FRAGMENT) {
            if (cursor + 1 == raw.length()) {
                uri->fragment = raw.substr(last, cursor - last);
                break;
            }
        }
    }
    return uri;
}

#ifndef VERSION

void test_parser(std::string url, std::string scheme, std::string userinfo, 
                 std::string host, int port, std::string path, std::string query, std::string fragment) {
    std::unique_ptr<URI> uri = parseUri(url);
    if (uri->scheme == scheme && uri->authority.userinfo == userinfo && 
        uri->authority.host == host && uri->authority.port == port &&
        uri->path == path && uri->query == query && uri->fragment == fragment) {
        std::cout << "noice" << std::endl;
    } else {
        std::cout << "nope" << std::endl;
        std::cout << uri->scheme << " vs " << scheme << std::endl;
        std::cout << uri->authority.userinfo << " vs " << userinfo << std::endl;
        std::cout << uri->authority.host << " vs " << host << std::endl;
        std::cout << uri->authority.port << " vs " << port << std::endl;
        std::cout << uri->path << " vs " << path << std::endl;
        std::cout << uri->query << " vs " << query << std::endl;
        std::cout << uri->fragment << " vs " << fragment << std::endl;
    }
}

int
main(void) {
    test_parser("HTTP://username:password@www.example.org:8080/",                 "http", "username:password", "www.example.org", 8080, "/", "", "");
    test_parser("http://username:password@www.example.org:8080/",                 "http", "username:password", "www.example.org", 8080, "/", "", "");
    test_parser("http://www.example.org:8080/",                                   "http", "",                  "www.example.org", 8080, "/", "", "");
    test_parser("http://www.example.org/",                                        "http", "",                  "www.example.org", 80, "/", "", "");
    test_parser("http://www.example.org",                                         "http", "",                  "www.example.org", 80, "/", "", "");
    //test_parser("http://www.example.org//",                                       "http", "",                  "www.example.org", 80, "/", "", "");
    test_parser("http://www.example.org/this/path",                               "http", "",                  "www.example.org", 80, "/this/path", "", "");
    test_parser("http://www.example.org:/this/path",                              "http", "",                  "www.example.org", 80, "/this/path", "", "");
    test_parser("http://username:password@www.example.org:/this/path",            "http", "username:password", "www.example.org", 80, "/this/path", "", "");
    test_parser("http://username:password@www.example.org/",                      "http", "username:password",  "www.example.org", 80, "/", "", "");
    test_parser("http://username:passwor:d@www.example.org/",                     "http", "username:passwor:d", "www.example.org", 80, "/", "", "");
    test_parser("http://username:passwor:d@www.example.org:9090/",                "http", "username:passwor:d", "www.example.org", 9090, "/", "", "");
    test_parser("http://username:passwor:d@www.example.org:9090/this/path?query", "http", "username:passwor:d", "www.example.org", 9090, "/this/path", "query", "");
    test_parser("http://www.example.org:9090/this/path?query",                    "http", "", "www.example.org", 9090, "/this/path", "query", "");
    test_parser("http://www.example.org/this/path?query",                         "http", "", "www.example.org", 80, "/this/path", "query", "");
    test_parser("http://www.example.org?query",                                   "http", "", "www.example.org", 80, "/", "query", "");
}
#endif
