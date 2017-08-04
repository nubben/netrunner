#ifndef NODE_H
#define NODE_H

#include "../graphics/components/Component.h"
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <sstream>
#include <iostream>

enum class NodeType {
    ROOT,
    TAG,
    TEXT
};

class Node {
public:
    Node(NodeType type);
    virtual ~Node();
    NodeType nodeType;
    std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
    std::shared_ptr<Component> component;

    virtual std::vector<std::string> getSourceList();
};

#endif
