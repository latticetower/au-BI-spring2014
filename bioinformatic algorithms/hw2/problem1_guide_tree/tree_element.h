#pragma once
#include"ivisitor.h"

class TreeElement {
  int _index; //index of leaf in fasta array
public:
  TreeElement():_index(-1) {}

  virtual ~TreeElement() {};
  virtual void acceptVisitor(IVisitor * visitor) = 0;

  void setIndex(int index) {
    _index = index;
  }
  int getIndex() const{
    return _index;
  }
};