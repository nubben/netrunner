#include "BrowsingHistory.h"

BrowsingHistory::BrowsingHistory() :
    history(),
    currentPosition(0) {}

void BrowsingHistory::addEntry(URL url) {
    if (!history.empty()) {
        ++currentPosition;
    }
    if (currentPosition < history.size()) {
        history.erase(history.begin() + currentPosition, history.end());
    }
    history.push_back(url);
}

URL const& BrowsingHistory::goForward() {
    if (currentPosition < history.size()) {
        ++currentPosition;
    }
    return history[currentPosition];
}

URL const& BrowsingHistory::goBack() {
    if (currentPosition > 0) {
        --currentPosition;
    }
    return history[currentPosition];
}

std::vector<URL> const& BrowsingHistory::getHistory() const {
    return history;
}
