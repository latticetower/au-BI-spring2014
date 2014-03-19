#pragma once
#include"ivisitor.h"

class TreeElement {
public:
  virtual ~TreeElement() {};
  virtual void acceptVisitor(IVisitor * visitor) = 0;
};