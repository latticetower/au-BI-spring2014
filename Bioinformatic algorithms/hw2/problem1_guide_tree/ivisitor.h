#pragma once 
#include <iostream>
/*im not sure is it useful in this case*/

class Leaf;
class Node;

class IVisitor{
public:
  virtual ~IVisitor() {};
  virtual void visit(Leaf const* leaf) = 0;
  virtual void visit(Node const* node) = 0;
};
