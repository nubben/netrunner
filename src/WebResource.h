#ifndef WEBRESOURCE_H
#define WEBRESOURCE_H

#include <string>

enum class ResourceType {
    INVALID,
    HTML,
    CSS,
    JS
};

class WebResource {
public:
    WebResource();
    WebResource(ResourceType rtype, std::string const& rraw);

    ResourceType resourceType;
    std::string raw;
};

// Depending on the resourceName specified, this function will forward the call
// to either getLocalWebResource or getOnlineWebResource.
WebResource getWebResource(std::string resourceName);

// Loads a resource from the local file storage.
WebResource getLocalWebResource(std::string fileName);

// Loads a resource from an internet address.
WebResource getOnlineWebResource(std::string url);

#endif
