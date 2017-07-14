#ifndef COMPONENTBUILDER_H
#define COMPONENTBUILDER_H

#include <algorithm>
#include <memory>
#include <unordered_map>
#include "Component.h"
#include "../elements/AElement.h"
#include "../elements/BLOCKQUOTEElement.h"
#include "../elements/H1Element.h"
#include "../elements/H2Element.h"
#include "../elements/H3Element.h"
#include "../elements/LIElement.h"
#include "../elements/PElement.h"
#include "../elements/SPANElement.h"
#include "../../html/TagNode.h"
#include "../../html/TextNode.h"
#include "../../html/Node.h"

typedef std::unordered_map<std::string, std::function<std::unique_ptr<Component>(const Node &node, int y, int windowWidth, int windowHeight)>> ElementRendererMap;

class ComponentBuilder {
private:
    const static ElementRendererMap tagRenderers;
    const static ElementRendererMap textRenderers;
public:
    std::unique_ptr<Component> build(const std::shared_ptr<Node> node, int y, int windowWidth, int windowHeight);
};

#endif