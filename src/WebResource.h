#ifndef WEBRESOURCE_H
#define WEBRESOURCE_H

#include <string>
#include "URL.h"

enum class ResourceType {
    INVALID,
    HTML,
    CSS,
    JS
};

struct WebResource {
    WebResource();
    WebResource(ResourceType rtype, std::string const& rraw);

    ResourceType resourceType;
    std::string raw;
};

// Depending on the resourceName specified, this function will forward the call
// to either getLocalWebResource or getOnlineWebResource.
WebResource getWebResource(URL const& url);

// Loads a resource from the local file storage.
WebResource getLocalWebResource(URL const& url);

// Loads a resource from an internet address.
WebResource getOnlineWebResource(URL const& url);

#endif
