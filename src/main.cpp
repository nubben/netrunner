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
#include "CommandLineParams.h"
#include "URL.h"

const std::unique_ptr<Window> window = std::make_unique<Window>();
URL currentURL;

bool setWindowContent(URL const& url);

bool setWindowContent(URL const& url) {
    WebResource res = getWebResource(url);
    std::cout << "RESTYPE: " << (int)res.resourceType << std::endl;
    if (res.resourceType == ResourceType::INVALID) {
        std::cout << "Invalid resource type: " << res.raw << std::endl;
        return false;
    }

    HTMLParser parser;
    const std::clock_t begin = clock();
    std::shared_ptr<Node> rootNode = parser.parse(res.raw);
    const std::clock_t end = clock();
    std::cout << "Parsed document in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
    window->setDOM(rootNode);
    return true;
}

void navTo(std::string url) {
    std::cout << "go to: " << url << std::endl;
    currentURL = currentURL.merge(URL(url));
    std::cout << "go to: " << currentURL << std::endl;
    setWindowContent(currentURL);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "./netrunner <url|file.html>" << std::endl;
        return 1;
    }
    initCLParams(argc, argv);
    std::cout << "/g/ntr - NetRunner build " << __DATE__ << std::endl;
    std::string url = getCLParamByIndex(1);
    if (url[0] == '/') {
        url = "file://" + url;
    }
    currentURL=URL(url);
    std::cout << "loading [" << currentURL << "]" << std::endl;

    if (!setWindowContent(currentURL)) {
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
