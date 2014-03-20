#pragma once
#include <string>
#include <memory>
#include "tree_element.h"
#include "ivisitor.h"

#include "fasta_data.h"
#include "computations.h"



class Node: public TreeElement{
  std::shared_ptr<TreeElement> _op1, _op2;
public:
  Node(int index, TreeElement* op1, TreeElement* op2): _op1(op1), _op2(op2) {
    setIndex(index);
  }
  Node(int index, std::shared_ptr<TreeElement> op1, std::shared_ptr<TreeElement> op2): _op1(op1), _op2(op2) {
    setIndex(index);
  }
  ~Node() {
  }
  void acceptVisitor(IVisitor * visitor) {
    return visitor->visit(this);
  }
  std::shared_ptr<TreeElement> const & op1() const {
    return _op1;
  }
  std::shared_ptr<TreeElement> const & op2() const {
    return _op2;
  }
};