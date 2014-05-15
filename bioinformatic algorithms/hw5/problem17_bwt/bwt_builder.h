#pragma once
#include <sstream>
#include "ivisitor.h"
#include "suffix_tree.h"


class BWTBuilder: public IVisitor {
  std::vector<long> bwt;
public:
  BWTBuilder() {
    bwt.clear();
  }
  ~BWTBuilder() {
  }

  void visit(SuffixTree * _tree) {
    //string_data << "tree start: " << std::endl;
    _tree->getRoot()->acceptVisitor(this);
    //string_data << "tree end";
  }

  void visit(tree_vertex<SuffixTree> * node) {
    //string_data << "(";
    //string_data << "->" << node->to_s() << std::endl;
    if (node->getNext() != NULL) { 
      
      //string_data << "->" << node->getNext()->to_s() << ";" << std::endl;
      node->getNext()->acceptVisitor(this);
      tree_vertex<SuffixTree> * nxt = node->getNext()->getNeighbour();
      while (nxt != NULL) {
        
        //string_data << nxt->to_s() << ";" << std::endl;
        nxt->acceptVisitor(this);
        nxt = nxt->getNeighbour();
      }
    }
    //string_data << ")";
      
  }

  const std::vector<long> getBWT() const{
    return bwt;
  }
};