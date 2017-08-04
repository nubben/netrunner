#ifndef TAGNODE_H
#define TAGNODE_H

#include "Node.h"
#include <map>

class TagNode : public Node {
public:
    TagNode();
    std::string tag;
    std::map<std::string, std::string> properties;

    std::vector<std::string> getSourceList() override;
};

#endif
