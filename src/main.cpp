#include "graphics/opengl/Window.h"
#include "html/HTMLParser.h"
#include <ctime>
#include <iostream>
#include "WebResource.h"
#include "CommandLineParams.h"
#include "URL.h"
#include "Log.h"

const std::unique_ptr<Window> window = std::make_unique<Window>();
//URL currentURL;

bool setWindowContent(URL const& url) {
    WebResource res = getWebResource(url);
    if (res.resourceType == ResourceType::INVALID) {
        logError() << "Invalid resource type: " << res.raw << std::endl;
        return false;
    }

    HTMLParser parser;
    const std::clock_t begin = clock();
    std::shared_ptr<Node> rootNode = parser.parse(res.raw);
    const std::clock_t end = clock();
    logDebug() << "Parsed document in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
    window->setDOM(rootNode);
    return true;
}

/*
void navTo(std::string url) {
    logDebug() << "navTo(" << url << ")" << std::endl;
    currentURL = currentURL.merge(URL(url));
    logDebug() << "go to: " << currentURL << std::endl;
    setWindowContent(currentURL);
    
    //std::shared_ptr<URI> uri = parseUri(url);
    //const std::shared_ptr<HTTPRequest> request = std::make_shared<HTTPRequest>(uri);
    //currentURL=url;
    //request->sendRequest(handleRequest);
}
*/

/*
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
        const std::unique_ptr<HTTPRequest> request = std::make_unique<HTTPRequest>(uri);
        request->sendRequest(handleRequest);
        return;
    }
    else {
        std::cout << "Unknown Status Code: " << response.statusCode << std::endl;
    }
}
*/

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "./netrunner <url|file.html> [-log <error|warning|notice|info|debug>]" << std::endl;
        return 1;
    }
    std::cout << "/g/ntr - NetRunner build " << __DATE__ << std::endl;

    initCLParams(argc, argv);
    std::string url = getCLParamByIndex(1);
    if (url[0] == '/') {
        url = "file://" + url;
    }
    //currentURL=URL(url);
    window->currentURL=URL(argv[1]);
    logDebug() << "loading [" << window->currentURL << "]" << std::endl;

    if (!setWindowContent(window->currentURL)) {
        return 1;
    }

    WebResource res = getWebResource(window->currentURL);
    if (res.resourceType == ResourceType::INVALID) {
        std::cout << "Invalid resource type" << std::endl;
        return 1;
    }

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
