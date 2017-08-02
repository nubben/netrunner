#include "WebResource.h"

#include <fstream>
#include <map>
#include <algorithm>
#include "networking/HTTPRequest.h"
#include "networking/HTTPResponse.h"
#include "StringUtils.h"
#include <iostream>

namespace {
    
    // only used for local files atm
    std::map<std::string, ResourceType> strToRT = {
        {"html", ResourceType::HTML},
        {"css", ResourceType::CSS},
        {"js", ResourceType::JS}
    };

    bool isOnlineResource(URL const& url) {
        return url.protocol != "file";
    }

}

WebResource::WebResource() {
    resourceType = ResourceType::INVALID;
    raw = "";
}

WebResource::WebResource(ResourceType rtype, std::string const& rraw) {
    resourceType = rtype;
    raw = rraw;
}

WebResource getWebResource(URL const& url) {
    if (isOnlineResource(url)) {
        //std::cout << "WebReousrce::getWebResource - isOnline" << std::endl;
        return getOnlineWebResource(url);
    }
    //std::cout << "WebReousrce::getWebResource - isOffline" << std::endl;
    return getLocalWebResource(url);
}

WebResource getLocalWebResource(URL const& url) {
    std::string fileExtension = getFilenameExtension(url.document);
    if (fileExtension.length() == 0) {
        return WebResource(ResourceType::INVALID,
                           "Could not find any file extension");
    }
    
    // Convert file extension to lowercase for table lookup.
    std::transform(fileExtension.begin(),
                   fileExtension.end(),
                   fileExtension.begin(),
                   tolower);

    if (strToRT.find(fileExtension) == strToRT.end()) {
        return WebResource(ResourceType::INVALID,
                           "Local file with extension " + fileExtension + " is not supported. Did you forget a http://?");
    }

    std::ifstream in(url.document, std::ios::in | std::ios::binary);
    if (in) {
        // There exists more efficient ways of doing this, but it works for the
        // time being.
        std::string contents = std::string(std::istreambuf_iterator<char>(in), 
                                           std::istreambuf_iterator<char>());
        
        return WebResource(strToRT[fileExtension], contents);
    }

    return WebResource(ResourceType::INVALID,
                       "Could not open file " + url.document);
}

WebResource getOnlineWebResource(URL const& url) {
    HTTPRequest request (url.host, url.document);
    WebResource returnRes;

    request.sendRequest([&](HTTPResponse const& response){
        if (response.statusCode != 200) {
            returnRes.resourceType = ResourceType::INVALID;
            returnRes.raw = "Unsupported status code";
        }
        // TODO: Set resourceType based on Content-Type field.
        returnRes.resourceType = ResourceType::HTML;
        returnRes.raw = std::move(response.body);
    });

    return returnRes;
}
