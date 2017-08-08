#include "BrowsingHistory.h"

BrowsingHistory::BrowsingHistory(std::function<void(URL const&)> onGotoPage_) :
    history(),
    currentPosition(history.end()),
    onGotoPage(onGotoPage_) {}

size_t BrowsingHistory::length() const {
    return history.size();
}

void BrowsingHistory::back() {
    go(-1);
}

void BrowsingHistory::forward() {
    go(1);
}

void BrowsingHistory::go() {
    go(0);
}

void BrowsingHistory::go(int diff) {
    if (history.size() == 0) {
        return;
    }
    std::vector<URL>::iterator newPos = currentPosition + diff;
    if (newPos >= history.begin() && newPos < history.end()) {
        currentPosition = newPos;
        onGotoPage(*currentPosition);
    }
}

void BrowsingHistory::pushState(void* stateObj, std::string const& title, URL const& url) {
    if (history.size() == 0) {
        history.push_back(url);
        currentPosition = history.begin();
        return;
    }
    URL currentURL = *currentPosition;
    history.erase(++currentPosition, history.end());
    history.push_back(currentURL.merge(url));
    currentPosition = history.end() - 1;
}

void BrowsingHistory::replaceState(void* stateObj, std::string const& title, URL const& url) {
    if (history.size() == 0) {
        return;
    }
    *currentPosition = (*currentPosition).merge(url);
}
