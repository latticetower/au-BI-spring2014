#pragma once
#include <sstream>
#include "ivisitor.h"
#include "suffix_tree.h"
typedef std::set<std::shared_ptr<tree_vertex<SuffixTree> >, tree_vertex<SuffixTree>::classcomp> NextVerticesContainer;


class BWTBuilder: public IVisitor {
  std::vector<long> bwt;
public:
  BWTBuilder() {
    bwt.clear();
  }
  ~BWTBuilder() {
  }

  
  void visit(SuffixTree * _tree) {
    //string_data << "tree start:" << std::endl;
    _tree->getRoot()->acceptVisitor(this);
    //string_data << "tree end";
  }

  void visit(tree_vertex<SuffixTree> * node) {
    //string_data << "(";
    //string_data << node->get_value() << "->" << std::endl;
    
    for (NextVerticesContainer::iterator iter = node->next_vertices.begin(); iter != node->next_vertices.end(); ++iter) {  
      (*iter)->acceptVisitor(this);
    }
    if (node->is_leaf) {
      bwt.push_back((node->index > 0 ? node->index : node->string_length() ) - 1);
    }
    //string_data << ")";
      
  }

  const std::vector<long> getBWT() const{
    return bwt;
  }
};