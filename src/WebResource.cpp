#include "WebResource.h"

#include <fstream>
#include <map>
#include <algorithm>
#include "networking/HTTPRequest.h"
#include "networking/HTTPResponse.h"
#include "StringUtils.h"
#include <iostream>
#include "Log.h"

namespace {
    
    // only used for local files atm
    std::map<std::string, ResourceType> strToRT = {
        {"html", ResourceType::HTML},
        {"css", ResourceType::CSS},
        {"js", ResourceType::JS}
    };

    bool isOnlineResource(URL const& url) {
        return url.scheme != "file";
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
    std::string fileExtension = getFilenameExtension(url.path);
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

    std::ifstream in(url.path, std::ios::in | std::ios::binary);
    if (in) {
        // There exists more efficient ways of doing this, but it works for the
        // time being.
        std::string contents = std::string(std::istreambuf_iterator<char>(in), 
                                           std::istreambuf_iterator<char>());
        
        return WebResource(strToRT[fileExtension], contents);
    }

    return WebResource(ResourceType::INVALID,
                       "Could not open file " + url.path);
}

WebResource getOnlineWebResource(URL const& url) {
    std::shared_ptr<URL> uri=std::make_shared<URL>(url);
    HTTPRequest request (uri);

    //window->currentURL=url;
    //request->sendRequest(handleRequest);
    
    WebResource returnRes;

    std::string redirectLocation = "";

    request.sendRequest([&](HTTPResponse const& response){
        logDebug() << "getOnlineWebResource request.sendRequest" << std::endl;
        if (response.statusCode == 301) {
            std::string location;
            if (response.properties.find("Location")==response.properties.end()) {
                if (response.properties.find("location")==response.properties.end()) {
                    logDebug() << "getOnlineWebResource - got 301 without a location" << std::endl;
                    for(auto const &row : response.properties) {
                        logDebug() << "getOnlineWebResource - " << row.first << "=" << response.properties.at(row.first) << std::endl;
                    }
                    redirectLocation = "_";
                } else {
                    location = response.properties.at("location");
                }
            } else {
                location = response.properties.at("Location");
            }
            logDebug() << "Redirect To: " << location << std::endl;
            redirectLocation = location;
        } else if (response.statusCode != 200) {
            returnRes.resourceType = ResourceType::INVALID;
            returnRes.raw = "Unsupported status code";
        } else {
            // TODO: Set resourceType based on Content-Type field.
            returnRes.resourceType = ResourceType::HTML;
            returnRes.raw = std::move(response.body);
        }
    });

    if (redirectLocation.size() > 0) {
        if (redirectLocation == "_") {
            return WebResource(ResourceType::INVALID,
                               "Got a 301 without a location");
        }
        return getOnlineWebResource(URL(redirectLocation));
    }

    return returnRes;
}
