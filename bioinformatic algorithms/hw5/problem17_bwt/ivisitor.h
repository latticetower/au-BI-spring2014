#pragma once 
#include <iostream>
/*im not sure is it useful in this case*/


class SuffixTree;
template<class T>
class tree_vertex;

class IVisitor{
public:
  virtual ~IVisitor() {};
  virtual void visit(tree_vertex<SuffixTree> * _vertex) = 0;
  virtual void visit(SuffixTree * _tree) = 0;
};
