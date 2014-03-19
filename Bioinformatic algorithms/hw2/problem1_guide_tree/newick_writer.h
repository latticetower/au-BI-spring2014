#pragma once
#include <sstream>
#include "ivisitor.h"
#include "fasta_data.h"
#include "guide_tree.h"

class NewickWriter: public IVisitor {
  GuideTree * _tree;
  FastaData * _fasta_data;
  std::stringstream string_data;
public:
  NewickWriter(GuideTree * guide_tree, FastaData* fasta_data):_tree(guide_tree), _fasta_data(fasta_data) {
    string_data.clear();
  }
  ~NewickWriter() {
  }
  void visit(Leaf const* leaf) {
    string_data << _fasta_data->data[leaf->getIndex()].first;
  }
  void visit(Node const* node) {
    string_data << "(";
    node->op1()->acceptVisitor(this);
    string_data << " : " << _tree->getDistance(node->op1()->getIndex(), node->op2()->getIndex()) << " : ";
    node->op2()->acceptVisitor(this);
    string_data << ")";
  }

  const std::string getResult() const{
    return string_data.str();
  }
};