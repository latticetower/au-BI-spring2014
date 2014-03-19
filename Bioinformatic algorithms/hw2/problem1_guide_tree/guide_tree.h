#pragma once
#include <string>
#include <map>
#include "tree_element.h"
#include "ivisitor.h"

#include "fasta_data.h"
#include "computations.h"


/* this class should build tree and contain basic tree structure */
class Leaf: public TreeElement{
  int _index;//index of leaf in fasta array
public:
  Leaf(int index): _index(index) {
  }
  ~Leaf() {
  }
  void acceptVisitor(IVisitor * visitor) {
    return visitor->visit(this);
  }
  int get() const {
    return _index;
  }
};
class Node: public TreeElement{
  std::unique_ptr<TreeElement> _op1, _op2;
public:
  Node(TreeElement* op1, TreeElement* op2): _op1(op1), _op2(op2) {
  }
  ~Node() {
  }
  void acceptVisitor(IVisitor * visitor) {
    return visitor->visit(this);
  }
   std::unique_ptr<TreeElement> const & op1() const {
    return _op1;
  }
  std::unique_ptr<TreeElement> const & op2() const {
    return _op2;
  }
};

class GuideTree {
  FastaData * fasta_data;//contains link to fasta array
  std::vector<std::unique_ptr<TreeElement> > _nodes;
  std::map<std::pair<int, int>, int> pair_distances;
  std::map<int, int> precomputed_sums;
public:
  GuideTree(FastaData * fasta): fasta_data(fasta) {  }
  //method prepares Leaf objects for future usage
  void PrepareLeaves() {
    for (int i = 0; i < fasta_data->data.size(); i++) {
      _nodes.push_back(std::unique_ptr<TreeElement>(new Leaf(i)));
    }

    for (int i = 0; i < fasta_data->data.size(); i++) {
      // fasta_data->data[i].second
      for (int j = i + 1; j < fasta_data->data.size(); j ++) {
        int dist = levenshtein_distance(fasta_data->data[i].second, fasta_data->data[j].second);
        precomputed_sums[i] += dist;
        precomputed_sums[j] += dist;
        pair_distances[std::make_pair(i, j)] = dist;
      }
      
    }
  }
  int getDistance(int i, int j) {
    if (i > j)
      return getDistance(j, i);
    if (i == j)
      return -1;
    return pair_distances[std::make_pair(i, j)];
  }

  std::pair<int, int> BuildQMatrix() {
    std::map<std::pair<int, int>, int > qvalues;
    int r = fasta_data->data.size(); 
   
    for (int i = 0; i < r; i ++) {
      for (int j = i + 1; j < r; j ++) {
        if (i != j) {
          qvalues[std::pair<int, int>(i, j)] = r*this->getDistance(i, j) - precomputed_sums[i] - precomputed_sums[j]; 
        }
      }
    }


  }
  void Build() {
    PrepareLeaves();
  }
  void acceptVisitor(IVisitor* visitor) {
    for (std::vector<std::unique_ptr<TreeElement> >::iterator iter = _nodes.begin(); iter != _nodes.end(); ++iter) {
      (*iter)->acceptVisitor(visitor);
    }
  }
};