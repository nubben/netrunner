#include "WebResource.h"

#include <fstream>
#include <map>
#include <algorithm>
#include "networking/HTTPRequest.h"
#include "networking/HTTPResponse.h"
#include "StringUtils.h"

namespace {

std::map<std::string, ResourceType> strToRT = {
    {"html", ResourceType::HTML},
    {"css", ResourceType::CSS},
    {"js", ResourceType::JS}
};

}

WebResource::WebResource() {
    resourceType = ResourceType::INVALID;
    raw = "";
}

WebResource::WebResource(ResourceType rtype, std::string const& rraw) {
    resourceType = rtype;
    raw = rraw;
}

WebResource getWebResource(std::string resourceName) {
    return WebResource(ResourceType::INVALID, "Not implemented");
}

WebResource getLocalWebResource(std::string fileName) {
    std::string fileExtension = getFilenameExtension(fileName);
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
                           "Resource type " + fileExtension + " not supported");
    }

    std::ifstream in(fileName, std::ios::in | std::ios::binary);
    if (in) {
        // There exists more efficient ways of doing this, but it works for the
        // time being.
        std::string contents = std::string(std::istreambuf_iterator<char>(in), 
                                           std::istreambuf_iterator<char>());
        
        return WebResource(strToRT[fileExtension], contents);
    }

    return WebResource(ResourceType::INVALID,
                       "Could not open file " + fileName);
}

WebResource getOnlineWebResource(std::string url) {
    return WebResource(ResourceType::INVALID, "Not implemented");
}
