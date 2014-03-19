#pragma once
#include <string>
#include "tree_element.h"
#include "ivisitor.h"

/* this class should build tree and contain basic tree structure */
class Leaf: public TreeElement{
public:
  Leaf(int index) {
    this->setIndex(index);
  }
  ~Leaf() {
  }
  void acceptVisitor(IVisitor * visitor) {
    return visitor->visit(this);
  }
};