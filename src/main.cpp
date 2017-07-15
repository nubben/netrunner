#include "graphics/opengl/Window.h"
#include "html/HTMLParser.h"
#include "html/TagNode.h"
#include "html/TextNode.h"
#include "networking/HTTPRequest.h"
#include "networking/HTTPResponse.h"
#include <ctime>
#include <iostream>
#include <memory>

const std::string getDocumentFromURL(const std::string &url);
const std::string getHostFromURL(const std::string &url);
void handleRequest(const HTTPResponse &response);

const std::unique_ptr<Window> window = std::make_unique<Window>();

const std::string getDocumentFromURL(const std::string &url) {
    int slashes = 0;
    for (unsigned int i = 0; i < url.length(); i++) {
        if (url[i] == '/') {
            slashes++;
            if (slashes == 3) {
                return url.substr(i, url.length() - i);
            }
        }
    }
    return "";
}

const std::string getHostFromURL(const std::string &url) {
    int slashes = 0;
    unsigned int start = 0;
    for (unsigned int i = 0; i < url.length(); i++) {
        if (url[i] == '/') {
            if (slashes == 2) {
                return url.substr(start, i - start);
            }
            slashes++;
            start = i + 1;
        }
    }
    return "";
}

void handleRequest(const HTTPResponse &response) {
    if (response.statusCode == 200) {
        const std::unique_ptr<HTMLParser> parser = std::make_unique<HTMLParser>();
        const std::clock_t begin = clock();
        std::shared_ptr<Node> rootNode = parser->parse(response.body);
        const std::clock_t end = clock();
        std::cout << "Parsed document in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
        window->setDOM(rootNode);
    }
    else if (response.statusCode == 301) {
        const std::string location = response.properties.at("Location");
        std::cout << "Redirect To: " << location << std::endl;
        const std::unique_ptr<HTTPRequest> request = std::make_unique<HTTPRequest>(getHostFromURL(location), getDocumentFromURL(location));
        request->sendRequest(handleRequest);
        return;
    }
    else {
        std::cout << "Unknown Status Code: " << response.statusCode << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "./netrunner <url>" << std::endl;
        return 1;
    }
    const std::unique_ptr<HTTPRequest> request = std::make_unique<HTTPRequest>(getHostFromURL(argv[1]), getDocumentFromURL(argv[1]));
    request->sendRequest(handleRequest);
    window->init();
    while (!glfwWindowShouldClose(window->window)) {
        const std::clock_t begin = clock();
        window->render();
        const std::clock_t end = clock();
//        std::cout << '\r' << std::fixed << (((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) * 1000) << std::scientific << " ms/f    " << std::flush;
    }
    return 0;
}
