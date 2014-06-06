#pragma once
#include <sstream>
#include "ivisitor.h"
#include "suffix_tree.h"

typedef std::set<std::shared_ptr<tree_vertex<SuffixTree> >, tree_vertex<SuffixTree>::classcomp> NextVerticesContainer;
  
class NewickWriter: public IVisitor {
  std::stringstream string_data;
public:
  NewickWriter() {
    string_data.clear();
  }
  ~NewickWriter() {
  }

  void visit(SuffixTree * _tree) {
    string_data << "tree start:" << std::endl;
    _tree->getRoot()->acceptVisitor(this);
    string_data << "tree end";
  }

  void visit(tree_vertex<SuffixTree> * node) {
    string_data << "(";
    string_data << node->get_value() << "->" <<  (node->is_leaf ? "leaf" : "") << std::endl;
    for (NextVerticesContainer::iterator iter = node->next_vertices.begin(); iter != node->next_vertices.end(); ++iter) {  
      (*iter)->acceptVisitor(this);
    }
    string_data << ")";
      
  }

  const std::string getResult() const{
    return string_data.str();
  }
};