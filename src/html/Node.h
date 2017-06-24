#ifndef NODE_H
#define NODE_H

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
    Node *parent;
    std::vector<Node*> children;
};

#endif
