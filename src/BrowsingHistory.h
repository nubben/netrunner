#ifndef BROWSINGHISTORY_H
#define BROWSINGHISTORY_H

#include <vector>
#include "URL.h"
#include <functional>

/*
  NOTE: The first parameter of the constructor is the callback to be called when
        calling go(int) (or back or forward or whatever). Should be a
        function that navigates to the specified URL.
 */

// modelled after https://developer.mozilla.org/en-US/docs/Web/API/History
class BrowsingHistory {
public:
    BrowsingHistory(std::function<void(URL const&)> onGotoPage_);

    unsigned int length() const;
    //void scrollRestoration(); // TODO: Implement this when it's relevant
    //void* state() const; // TODO: Implement when we have Javascript objects.
    
    void back();
    void forward();
    void go();
    void go(int diff);

    // stateObj: void* should be a Javascript object once we have those. Just
    //           give it nullptr for the time being.
    // title: "Firefox currently ignores this parameter, although it may
    //         use it in the future.". So i guess we ignore it too?
    void pushState(void* stateObj, std::string const& title, URL const& url);
    void replaceState(void* stateObj, std::string const& title, URL const& url);
    
private:
    std::vector<URL> history;
    std::vector<URL>::iterator currentPosition;
    std::function<void(URL const&)> onGotoPage;
};

#endif
