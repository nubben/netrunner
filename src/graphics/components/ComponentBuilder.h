#ifndef COMPONENTBUILDER_H
#define COMPONENTBUILDER_H

#include <algorithm>
#include <memory>
#include <unordered_map>
#include "Component.h"
#include "../elements/Element.h"
#include "../elements/AElement.h"
#include "../elements/BLOCKQUOTEElement.h"
#include "../elements/H1Element.h"
#include "../elements/H2Element.h"
#include "../elements/H3Element.h"
#include "../elements/LIElement.h"
#include "../elements/PElement.h"
#include "../elements/SPANElement.h"
#include "../elements/DIVElement.h"
#include "../elements/STRONGElement.h"
#include "../elements/INPUTElement.h"
#include "../../html/TagNode.h"
#include "../../html/TextNode.h"
#include "../../html/Node.h"

typedef std::unordered_map<std::string, std::function<std::unique_ptr<Component>(const Node &node, int y, int windowWidth, int windowHeight)>> ElementRendererMap;

class ComponentBuilder {
private:
    const static std::unordered_map<std::string, std::shared_ptr<Element>> elementMap;
public:
    std::shared_ptr<Component> build(const std::shared_ptr<Node> node, const std::shared_ptr<Component> &parentComponent, int windowWidth, int windowHeight);
};

std::string typeOfComponent(const std::shared_ptr<Component> &component);

#endif
