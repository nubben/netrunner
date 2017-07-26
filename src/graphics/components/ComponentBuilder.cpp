#include "ComponentBuilder.h"
#include <iostream>

std::pair<int, int> getPos(const std::shared_ptr<Component> &parent, bool isInline, int windowWidth);

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

std::pair<int, int> getPos(const std::shared_ptr<Component> &parent, bool isInline, int windowWidth) {
    // get last component
    std::pair<int, int> res;
    if (!parent->children.size()) {
        res.first=parent->x;
        res.second=parent->y;
        return res;
    }
    std::shared_ptr<Component> prev = parent->children.back();
    int x = parent->x;
    int y = parent->y;
    if (prev) {
        // 2nd or last
        if (prev->isInline) {
            // last was inline
            if (isInline) {
                x = prev->x + prev->width;
                y = prev->y; // keep on same line
            } else {
                // we're block
                y = prev->y - prev->height;
            }
        } else {
            // last was block
            y = prev->y - prev->height;
        }
        // really only inline but can't hurt block AFAICT
        if (x == windowWidth) {
            x=0;
            y-=prev->height; // how far down do we need to wrap?, the previous height?
        }
    } else {
        // first, there will be no width to add
    }
    //std::cout << "moving component to " << (int)x << "x" << (int)y << std::endl;
    res.first=x;
    res.second=y;
    return res;
}

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

    int x = parentComponent->x;
    int y = parentComponent->y - parentComponent->height;
    bool isInline = false;

    std::unordered_map<std::string, std::shared_ptr<Element>>::const_iterator elementPair = elementMap.find(tag);
    if (elementPair != elementMap.end()) {
        std::shared_ptr<Element> element = elementPair->second;
        isInline = parentComponent->isInline || element->isInline;
        if (dynamic_cast<TextNode*>(node.get())) {
            isInline = true;
            //TextNode *textNode = dynamic_cast<TextNode*>(node.get());
            //std::cout << "text node: " << textNode->text << std::endl;
            //std::cout << "text: " << textNode->text << std::endl;
        }
        std::tie(x,y)=getPos(parentComponent, isInline, windowWidth);
        //auto [x, y]=getPos(parentComponent, isInline);
        //std::cout << "Placing at " << (int)x << "x" << (int)y << " between " << windowWidth << "x" << windowHeight << std::endl;
        component = element->renderer(node, x, y, windowWidth, windowHeight);
        /*
        if (component) {
          std::cout << "component size: " << (int)component->width << "x" << (int)component->height << std::endl;
        } else {
          // no component eh?
        }
        */
    } else {
        //std::cout << "Unknown tag: " << tag << std::endl;
    }

    if (!component) {
        component = std::make_unique<Component>();
        component->x = x;
        component->y = y;
    }
    component->isInline = isInline;

    return component;
}
