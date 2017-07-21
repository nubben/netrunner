#include "HTMLParser.h"
#include "TextNode.h"
#include <algorithm>
#include <iostream>
#include <memory>

void printNode(const std::shared_ptr<Node> node, const int indent);

void printNode(const std::shared_ptr<Node> node, const int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << '\t';
    }

    if (node->nodeType == NodeType::ROOT) {
        std::cout << "ROOT\n" << std::endl;
    }
    else if (node->nodeType == NodeType::TAG) {
        std::cout << "TAG: " << dynamic_cast<TagNode*>(node.get())->tag << std::endl;
        for (const std::pair<std::string, std::string> property : dynamic_cast<TagNode*>(node.get())->properties) {
            for (int i = 0; i < indent; i++) {
                std::cout << '\t';
            }
            std::cout << "  " << property.first << ": " << property.second << std::endl;
        }
    }
    else if (node->nodeType == NodeType::TEXT) {
        std::cout << "TEXT: " << dynamic_cast<TextNode*>(node.get())->text << std::endl;
    }

    for (std::shared_ptr<Node> child : node->children) {
        printNode(child, indent + 1);
    }
}

std::shared_ptr<Node> HTMLParser::parse(const std::string &html) const {
    std::shared_ptr<Node> rootNode = std::make_shared<Node>(NodeType::ROOT);
    std::shared_ptr<Node> currentNode = rootNode;
    std::vector<unsigned int> starts;
    unsigned int cursor;
    int state = 0;
    int prependWhiteSpace=false;
    for (cursor = 0; cursor < html.length(); cursor++) { // TODO handle trying to look ahead past string
        if (state == 0) { // Neutral
            if (html[cursor] == ' ' || html[cursor] == '\t' || html[cursor] == '\r' || html[cursor] == '\n') {
                prependWhiteSpace=true;
                continue;
            }
            else if (html[cursor] == '<') {
                if (html[cursor + 1] == '!') {
                    state = 1;
                }
                else if (html[cursor + 1] == '/') {
                    if (currentNode && currentNode->parent) {
                      currentNode = currentNode->parent;
                    } else {
                      std::cout << "HTMLParser::Parse - currentNode/parent is null - close tag" << std::endl;
                    }
                    state = 1;
                }
                else if (
                    (html[cursor + 1] == 'h' && html[cursor + 2] == 'r') ||
                    (html[cursor + 1] == 'b' && html[cursor + 2] == 'r') ||
                    (html[cursor + 1] == 'w' && html[cursor + 2] == 'b' && html[cursor + 3] == 'r') ||
                    (html[cursor + 1] == 'i' && html[cursor + 2] == 'm' && html[cursor + 3] == 'g') ||
                    (html[cursor + 1] == 'l' && html[cursor + 2] == 'i' && html[cursor + 3] == 'n' && html[cursor + 4] == 'k') ||
                    (html[cursor + 1] == 'm' && html[cursor + 2] == 'e' && html[cursor + 3] == 't' && html[cursor + 4] == 'a') ||
                    (html[cursor + 1] == 'i' && html[cursor + 2] == 'n' && html[cursor + 3] == 'p' && html[cursor + 4] == 'u' && html[cursor + 5] == 't')
                    ) {
                    state = 1;
                }
                else {
                    std::shared_ptr<TagNode> tagNode = std::make_shared<TagNode>();
                    if (currentNode) {
                      currentNode->children.push_back(tagNode);
                      tagNode->parent = currentNode;
                    } else {
                      std::cout << "HTMLParser::Parse - currentNode is null - tagNode" << std::endl;
                    }
                    currentNode = tagNode;
                    starts.push_back(cursor);
                    state = 2;
                }
            }
            else {
                std::shared_ptr<TextNode> textNode = std::make_shared<TextNode>();
                // not sure why currentNode is null but it is
                if (currentNode) {
                  currentNode->children.push_back(textNode);
                  textNode->parent = currentNode;
                } else {
                  std::cout << "HTMLParser::Parse - currentNode is null - textNode" << std::endl;
                }
                currentNode = textNode;
                starts.push_back(cursor);
                state = 3;
            }
            cursor--;
        }
        else if (state == 1) { // Skip Over Element
            if (html[cursor] == '>') {
                state = 0;
                prependWhiteSpace=false;
            }
        }
        else if (state == 2) { // Tag
            if (html[cursor] == '>') {
                std::string element = html.substr(starts.back(), cursor - starts.back() + 1);
                starts.pop_back();
                parseTag(element, *dynamic_cast<TagNode*>(currentNode.get()));
                state = 0;
                prependWhiteSpace=false;
            }
        }
        else if (state == 3) { // Text
            if (html[cursor + 1] == '<') {
                dynamic_cast<TextNode*>(currentNode.get())->text = (prependWhiteSpace?" ":"") + html.substr(starts.back(), cursor - starts.back() + 1);
                starts.pop_back();
                if (currentNode && currentNode->parent) {
                  currentNode = currentNode->parent;
                } else {
                  std::cout << "HTMLParser::Parse - currentNode/parent is null - textNode state3" << std::endl;
                }
                state = 0;
                prependWhiteSpace=false;
            }
        }
    }

    //printNode(rootNode, 0);
    return rootNode;
}

void HTMLParser::parseTag(const std::string &element, TagNode &tagNode) const {
    unsigned int cursor;
    unsigned int start = 1; // skip first <
    int state = 0;
    std::string propertyKey;
    for (cursor = 0; cursor < element.length();  cursor++) {
        if (state == 0) {
            if (element[cursor] == ' ' || element[cursor] == '>') {
                tagNode.tag = element.substr(start, cursor - start);
                std::transform(tagNode.tag.begin(), tagNode.tag.end(), tagNode.tag.begin(), tolower);
                start = cursor + 1;
                state = 1;
            }
        }
        else if (state == 1) {
            if (element[cursor] == ' ') {
                start = cursor + 1;
            }
            else if (element[cursor] == '=') {
                propertyKey = element.substr(start, cursor - start);
                state = 2;
            }
        }
        else if (state == 2) {
            if (element[cursor] == '"') {
                start = cursor + 1;
                state = 3;
            }
            else if (element[cursor] == '\'') {
                start = cursor + 1;
                state = 4;
            }
        }
        else if (state == 3) {
            if (element[cursor] == '"') {
                tagNode.properties.insert(std::pair<std::string, std::string>(propertyKey, element.substr(start, cursor - start)));
                start = cursor + 1;
                state = 1;
            }
        }
        else if (state == 4) {
            if (element[cursor] == '\'') {
                tagNode.properties.insert(std::pair<std::string, std::string>(propertyKey, element.substr(start, cursor - start)));
                start = cursor + 1;
                state = 1;
            }
        }
    }
}
