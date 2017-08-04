#include "graphics/opengl/Window.h"
#include "html/HTMLParser.h"
#include "html/TagNode.h"
#include "html/TextNode.h"
#include "networking/HTTPRequest.h"
#include "networking/HTTPResponse.h"
#include <ctime>
#include <iostream>
#include <memory>
#include "StringUtils.h"
#include "WebResource.h"

void handleRequest(const HTTPResponse &response);

const std::unique_ptr<Window> window = std::make_unique<Window>();
std::string currentURL="";

void navTo(std::string url) {
    std::cout << "go to: " << url << std::endl;
    //
    // All of this needs to be redone correctly
    // this is just temporary hack code, so I can get to specific pages for debugging
    //
    const std::size_t dSlashPos=url.find("//");
    if (dSlashPos!=std::string::npos) {
        // remote
        // check for relative
        if (dSlashPos==0) {
            // we only support http atm
            url="http:"+url;
        }
    } else {
        // https://github.com/unshiftio/url-parse/blob/master/index.js#L111
        //auto const lastSlashPos=url.find_last_of('/');
        // could collapse ./|/. to . and /./ to /
        if (url=="." || url=="./") {
            auto const lastSlashPos=getDocumentFromURL(currentURL).find_last_of('/');
            if (lastSlashPos!=std::string::npos) {
                url="http://"+getHostFromURL(currentURL)+getDocumentFromURL(currentURL).substr(0, lastSlashPos)+'/';
            }
        } else {
            if (url[0]=='/') {
                // absolute URL
                url="http://"+getHostFromURL(currentURL)+url;
            } else {
                // relative URL
                const std::string path=getDocumentFromURL(currentURL);
                std::cout << "old path: " << path << std::endl;
                if (url.length()>1 && url[0]=='.' && url[1]=='.') {
                    // are we at / ?
                    if (path=="/") {
                        // if so, remove ../
                        if (url.length()>2) {
                            url=url.substr(3);
                        } else {
                            url=url.substr(2);
                        }
                        url=path+url;
                    } else {
                        // .. but we're not in /
                        // make sure not a filename
                        auto const inDir=path.find_last_of("/");
                        if (inDir!=std::string::npos) {
                            // now take off a directory
                            auto const inDir2=path.substr(0, inDir).find_last_of("/");
                            url=path.substr(0, inDir2+1)+url.substr(2);
                        } else {
                            url=path+url;
                        }
                    }
                } else {
                    url=path+url;
                }
                url="http://"+getHostFromURL(currentURL)+url;
            }
        }
    }
    // strip # off
    std::size_t hashPos=url.find("#");
    if (hashPos!=std::string::npos) {
        url=url.substr(0, hashPos);
    }
    std::cout << "go to: " << url << std::endl;
    std::shared_ptr<Node> rootNode = std::make_shared<Node>(NodeType::ROOT);
    window->setDOM(rootNode);
    std::shared_ptr<URI> uri = parseUri(url);
    const std::shared_ptr<HTTPRequest> request = std::make_shared<HTTPRequest>(uri, getHostFromURL(url), getDocumentFromURL(url));
    currentURL=url;
    request->sendRequest(handleRequest);
}

void handleRequest(const HTTPResponse &response) {
    std::cout << "main:::handleRequest - statusCode: " << response.statusCode << std::endl;
    if (response.statusCode == 200) {
        const std::unique_ptr<HTMLParser> parser = std::make_unique<HTMLParser>();
        const std::clock_t begin = clock();
        std::shared_ptr<Node> rootNode = parser->parse(response.body);
        const std::clock_t end = clock();
        std::cout << "Parsed document in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
        window->setDOM(rootNode);
    }
    else if (response.statusCode == 301) {
        std::string location;
        if (response.properties.find("Location")==response.properties.end()) {
            if (response.properties.find("location")==response.properties.end()) {
                std::cout << "::handleRequest - got 301 without a location" << std::endl;
                for(auto const &row : response.properties) {
                    std::cout << "::handleRequest - " << row.first << "=" << response.properties.at(row.first) << std::endl;
                }
                return;
            } else {
                location = response.properties.at("location");
            }
        } else {
            location = response.properties.at("Location");
        }
        std::cout << "Redirect To: " << location << std::endl;
        std::shared_ptr<URI> uri = parseUri(location);
        const std::unique_ptr<HTTPRequest> request = std::make_unique<HTTPRequest>(uri, getHostFromURL(location), getDocumentFromURL(location));
        request->sendRequest(handleRequest);
        return;
    }
    else {
        std::cout << "Unknown Status Code: " << response.statusCode << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "./netrunner <url|file.html>" << std::endl;
        return 1;
    }
    std::cout << "/g/ntr - NetRunner build " << __DATE__ << std::endl;
    currentURL=argv[1];
    std::cout << "loading [" << currentURL << "]" << std::endl;

    WebResource res = getWebResource(currentURL);
    if (res.resourceType == ResourceType::INVALID) {
        std::cout << "Invalid resource type: " << res.raw << std::endl;
        return 1;
    }

    HTMLParser parser;
    const std::clock_t begin = clock();
    std::shared_ptr<Node> rootNode = parser.parse(res.raw);
    const std::clock_t end = clock();
    std::cout << "Parsed document in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
    window->setDOM(rootNode);

    window->init();
    if (!window->window) {
        return 1;
    }
    while (!glfwWindowShouldClose(window->window)) {
        //const std::clock_t begin = clock();
        window->render();
        glfwWaitEvents(); // block until something changes
        //const std::clock_t end = clock();
        //std::cout << '\r' << std::fixed << (((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) * 1000) << std::scientific << " ms/f    " << std::flush;
    }
    return 0;
}
