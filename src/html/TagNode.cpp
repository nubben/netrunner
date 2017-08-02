#include "TagNode.h"

TagNode::TagNode() : Node(NodeType::TAG) {
}

std::vector<std::string> TagNode::getSourceList() {
    std::vector<std::string> returnVec;
    
    auto propIter = properties.find("src");
    if (propIter != properties.end()) {
        returnVec.push_back(propIter->second);
    }

    for (std::shared_ptr<Node>& child : children) {
        auto childSrcs = std::move(child->getSourceList());
        returnVec.insert(returnVec.end(),
                         childSrcs.begin(),
                         childSrcs.end());
    }
    
    return returnVec;
}
