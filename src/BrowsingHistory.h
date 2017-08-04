#ifndef BROWSINGHISTORY_H
#define BROWSINGHISTORY_H

#include <vector>
#include "URL.h"

// if it's going to have fwd/back then
// please model after https://developer.mozilla.org/en-US/docs/Web/API/History
class BrowsingHistory {
public:
    BrowsingHistory();
    void addEntry(URL url);
    URL const& goForward();
    URL const& goBack();
    std::vector<URL> const& getHistory() const;
    
private:
    std::vector<URL> history;
    unsigned int currentPosition;
};

#endif
