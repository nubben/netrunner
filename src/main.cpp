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
    logDebug() << "main::setWindowContent - " << url << std::endl;
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

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "./netrunner <url|file.html> [-log <error|warning|notice|info|debug>]" << std::endl;
        return 1;
    }
    std::cout << "/g/ntr - NetRunner build " << __DATE__ << std::endl;

    initCLParams(argc, argv);
    std::string url = getCLParamByIndex(1);
    // if we do this here, shouldn't we do this in parseUri too?
    if (url.find("://") == url.npos) {
        url = "file://" + url;
    }
    //logDebug() << "pre URL parse [" << url << "]" << std::endl;
    window->currentURL=URL(url);
    logDebug() << "loading [" << window->currentURL << "]" << std::endl;
    
    if (!setWindowContent(window->currentURL)) {
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
