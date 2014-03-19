#pragma once
#include "ivisitor.h"
#include "fasta_data.h"
//#include "guide_tree.h"

class Printer: public IVisitor {
  FastaData * fasta_data;
public:
  Printer(FastaData* _data): fasta_data(_data) {

  } 
  void visit(Leaf const* leaf) {
    std::cout << fasta_data->data[leaf->getIndex()].first << ", " << fasta_data->data[leaf->getIndex()].second << std::endl;
  }

  void visit(Node const* node) {
    std::cout << "in node" << std::endl;
    node->op1()->acceptVisitor(this);
    std::cout << "second branch: " << std::endl;
    node->op2()->acceptVisitor(this);
  }
  ~Printer() {}
};