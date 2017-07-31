#include "ComponentBuilder.h"
#include <iostream>

const std::unordered_map<std::string, std::shared_ptr<Element>> ComponentBuilder::elementMap {
    {"a", std::make_shared<AElement>()},
    {"blockquote", std::make_shared<BLOCKQUOTEElement>()},
    {"h1", std::make_shared<H1Element>()},
    {"h2", std::make_shared<H2Element>()},
    {"h3", std::make_shared<H3Element>()},
    {"li", std::make_shared<LIElement>()},
    {"p", std::make_shared<PElement>()},
    {"span", std::make_shared<SPANElement>()},
    {"aside", std::make_shared<SPANElement>()},
    {"div", std::make_shared<DIVElement>()},
    {"br", std::make_shared<DIVElement>()},
    {"strong", std::make_shared<STRONGElement>()},
    {"b", std::make_shared<STRONGElement>()}
};

std::shared_ptr<Component> ComponentBuilder::build(const std::shared_ptr<Node> node, const std::shared_ptr<Component> &parentComponent, int windowWidth, int windowHeight) {
    std::shared_ptr<Component> component;
    std::string tag;

    if (node->nodeType == NodeType::TAG) {
        TagNode *tagNode = dynamic_cast<TagNode*>(node.get());
        if (tagNode) {
            tag = tagNode->tag;
        }
    }
    else if (node->nodeType == NodeType::TEXT) {
        TagNode *tagNode = dynamic_cast<TagNode*>(node->parent.get());
        if (tagNode) {
            tag = tagNode->tag;
        }
    }

    bool isInline = false;

    std::unordered_map<std::string, std::shared_ptr<Element>>::const_iterator elementPair = elementMap.find(tag);
    if (elementPair != elementMap.end()) {
        std::shared_ptr<Element> element = elementPair->second;
        isInline = parentComponent->isInline || element->isInline;
        // also it's Inline if it's a TextNode
        if (dynamic_cast<TextNode*>(node.get())) {
            isInline = true;
        }
        //std::tie(x,y) = getPos(parentComponent, isInline, windowWidth);
        //auto [x, y]=getPos(parentComponent, isInline);

        component = element->renderer(node, 0, 0, windowWidth, windowHeight); // doesn't always make a component
    } else {
        //std::cout << "Unknown tag: " << tag << std::endl;
    }

    if (!component) {
        component = std::make_unique<Component>();
    }
    /*
    TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
    if (textComponent) {
        std::cout << "compositing [" << textComponent->text << "]" << std::endl;
    }
    */
    //std::cout << "composting component, initial: " << (int)component->width << "x" << (int)component->height << std::endl;
    
    // set our available dimensions
    component->windowWidth = windowWidth;
    component->windowHeight = windowHeight;
    // set our type
    component->isInline = isInline;
    
    // place us in tree
    component->setParent(parentComponent);
    if (parentComponent) {
        if (parentComponent->children.size()) {
            component->previous = parentComponent->children.back();
        }
        parentComponent->children.push_back(component);
    }
    
    // figure out our position, size, texture
    component->layout();
    
    //std::cout << "post layout placed: " << (int)component->x << "x" << (int)component->y << " w/h: " << (int)component->width << "x" << (int)component->height << std::endl;
    return component;
}
