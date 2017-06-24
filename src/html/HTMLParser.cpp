#include "HTMLParser.h"
#include "TextNode.h"
#include <iostream>
#include <unistd.h>

void printNode(Node *node, int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << '\t';
    }
    if (node->nodeType == NodeType::ROOT) {
        std::cout << "ROOT" << std::endl;
    }
    if (node->nodeType == NodeType::TAG) {
        std::cout << "TAG: " << dynamic_cast<TagNode*>(node)->tag << std::endl;
        for (const std::pair<std::string, std::string> property : dynamic_cast<TagNode*>(node)->properties) {
            for (int i = 0; i < indent; i++) {
                std::cout << '\t';
            }
            std::cout << "  " << property.first << ": " << property.second << std::endl;
        }
    }
    else if (node->nodeType == NodeType::TEXT) {
        std::cout << "TEXT: " << dynamic_cast<TextNode*>(node)->text << std::endl;
    }

    for (Node *child : node->children) {
        printNode(child, indent + 1);
    }
}

void HTMLParser::parse(const std::string &html) const {
    Node rootNode = Node(NodeType::ROOT);
    Node *currentNode = &rootNode;
    std::vector<int> starts;
    int cursor;
    int start = 0;
    int state = 0;
    for (cursor = 0; cursor < html.length(); cursor++) { // TODO handle trying to look ahead past string
        if (state == 0) { // Neutral
            if (html[cursor] == ' ' || html[cursor] == '\t' || html[cursor] == '\r' || html[cursor] == '\n') {
                continue;
            }
            else if (html[cursor] == '<') {
                if (html[cursor + 1] == '!') {
                    state = 1;
                }
                else if (html[cursor + 1] == '/') {
                    currentNode = currentNode->parent;
                    state = 1;
                }
                else {
                    TagNode *tagNode = new TagNode();
                    currentNode->children.push_back(tagNode);
                    tagNode->parent = currentNode;
                    currentNode = tagNode;
                    starts.push_back(cursor);
                    state = 2;
                }
            }
            else {
                TextNode *textNode = new TextNode();
                currentNode->children.push_back(textNode);
                textNode->parent = currentNode;
                currentNode = textNode;
                starts.push_back(cursor);
                state = 3;
            }
        }
        else if (state == 1) { // Skip Over Element
            if (html[cursor] == '>') {
                state = 0;
            }
        }
        else if (state == 2) { // Tag
            if (html[cursor] == '>') {
                const int start = starts.back();
                starts.pop_back();
                std::string element = html.substr(start, cursor - start + 1);
                parseTag(element, dynamic_cast<TagNode*>(currentNode));
                state = 0;
            }
        }
        else if (state == 3) { // Text
            if (html[cursor + 1] == '<' && html[cursor + 2] == '/') {
                const int start = starts.back();
                starts.pop_back();
                dynamic_cast<TextNode*>(currentNode)->text = html.substr(start, cursor - start + 1);
                state = 0;
            }
        }
    }

    printNode(&rootNode, 0);
}

void HTMLParser::parseTag(const std::string &element, TagNode* tagNode) const {
    int cursor;
    int start = 1; // skip first <
    int state = 0;
    std::string propertyKey;
    for (cursor = 0; cursor < element.length();  cursor++) {
        if (state == 0) {
            if (element[cursor] == ' ' || element[cursor] == '>') {
                tagNode->tag = element.substr(start, cursor - start);
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
            if (element[cursor] == '\"') {
                start = cursor + 1;
                state = 3;
            }
        }
        else if (state == 3) {
            if (element[cursor] == '\"') {
                tagNode->properties.insert(std::pair<std::string, std::string>(propertyKey, element.substr(start, cursor - start)));
                start = cursor + 1;
                state = 1;
            }
        }
    }
}
