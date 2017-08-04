#include "Node.h"

Node::Node(NodeType type) {
    nodeType = type;
}

Node::~Node() {
}

std::vector<std::string> Node::getSourceList() {
    std::vector<std::string> returnVec;

    for (std::shared_ptr<Node>& child : children) {
        auto childSrcs = child->getSourceList();
        returnVec.insert(returnVec.end(),
                         childSrcs.begin(),
                         childSrcs.end());
    }
    
    return returnVec;
}
