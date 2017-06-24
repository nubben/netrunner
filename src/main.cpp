#include "graphics/opengl/Window.h"
#include "html/HTMLParser.h"
#include "html/TagNode.h"
#include "html/TextNode.h"
#include "networking/HTTPRequest.h"
#include "networking/HTTPResponse.h"
#include <iostream>
#include <memory>

const std::string getDocumentFromURL(const std::string &url) {
    int slashes = 0;
    for (int i = 0; i < url.length(); i++) {
        if (url[i] == '/') {
            slashes++;
            if (slashes == 3) {
                return url.substr(i, url.length() - i);
            }
        }
    }
}

const std::string getHostFromURL(const std::string &url) {
    int slashes = 0;
    int start = 0;
    for (int i = 0; i < url.length(); i++) {
        if (url[i] == '/') {
            if (slashes == 2) {
                return url.substr(start, i - start);
            }
            slashes++;
            start = i + 1;
        }
    }
}

void handleRequest(const HTTPResponse &response) {
    if (response.statusCode == 200) {
        const std::unique_ptr<HTMLParser> parser = std::unique_ptr<HTMLParser>(new HTMLParser());
        parser->parse(response.body);
    }
    else if (response.statusCode == 301) {
        const std::string location = response.properties.at("Location");
        std::cout << "Redirect To: " << location << std::endl;
        std::cout << getHostFromURL(location) << "!" << getDocumentFromURL(location) << std::endl;
        const std::unique_ptr<HTTPRequest> request = std::unique_ptr<HTTPRequest>(new HTTPRequest(getHostFromURL(location), getDocumentFromURL(location)));
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
    const std::unique_ptr<HTTPRequest> request = std::unique_ptr<HTTPRequest>(new HTTPRequest(getHostFromURL(argv[1]), getDocumentFromURL(argv[1])));
    request->sendRequest(handleRequest);
    const std::unique_ptr<Window> window = std::make_unique<Window>();
    window->init();
    while (!glfwWindowShouldClose(window->window)) {
        window->render();
    }
    return 0;
}
