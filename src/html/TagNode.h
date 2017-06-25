#ifndef TAGNODE_H
#define TAGNODE_H

#include "Node.h"
#include "../graphics/opengl/components/Component.h"
#include <map>

struct Element {
    std::string tag;
    std::function<std::unique_ptr<Component>(const Node &node, int y, int windowWidth, int windowHeight)> render;
};

class TagNode : public Node {
private:
    static const Element elements[];
public:
    TagNode();
    std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
    std::string tag;
    std::map<std::string, std::string> properties;
};

#endif
