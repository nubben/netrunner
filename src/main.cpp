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
#include "Log.h"

const std::unique_ptr<Window> window = std::make_unique<Window>();
URL currentURL;

bool setWindowContent(URL const& url);

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

void navTo(std::string url) {
    logDebug() << "navTo(" << url << ")" << std::endl;
    currentURL = currentURL.merge(URL(url));
    logDebug() << "go to: " << currentURL << std::endl;
    setWindowContent(currentURL);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "./netrunner <url|file.html>" << std::endl;
        return 1;
    }
    std::cout << "/g/ntr - NetRunner build " << __DATE__ << std::endl;
    
    initCLParams(argc, argv);
    std::string url = getCLParamByIndex(1);
    if (url[0] == '/') {
        url = "file://" + url;
    }
    currentURL=URL(url);
    logDebug() << "loading [" << currentURL << "]" << std::endl;

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
