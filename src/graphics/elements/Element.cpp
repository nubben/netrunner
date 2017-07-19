#include "Element.h"

Element::~Element() {
}

std::unique_ptr<Component> Element::renderer(const std::shared_ptr<Node> node, const int y, const int windowWidth, const int windowHeight) {
  return nullptr;
}