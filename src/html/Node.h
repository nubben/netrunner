#ifndef NODE_H
#define NODE_H

#include <memory>
#include <vector>

enum class NodeType {
    ROOT,
    TAG,
    TEXT
};

class Node {
public:
    Node(NodeType nodeType);
    virtual ~Node();
    NodeType nodeType;
    std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
};

#endif
