#pragma once
#include <memory>


//ititially we assume that all vertices describe the same string substrings
template <class Graph>
struct tree_vertex{
  //std::shared_ptr<tree_vertex> next[4];// use simple hash for storing next elements. we have fixed size alphabet, so can use it
  std::shared_ptr<tree_vertex> next_vertex; // really im not sure now what to store
  std::shared_ptr<tree_vertex> neighbour; //next neighbour for current vertex
  size_t start_pos, _length;
  
  Graph * owner;

  void set_parent(Graph * g) {
    owner = g;
  }

  
  tree_vertex(Graph * g, size_t start, size_t length = 1): next_vertex(0), neighbour(0), start_pos(start), _length(length), owner(g) {  }
  tree_vertex(tree_vertex & orig) {
    this->start_pos = orig->start_pos;
    this->_length = orig->_length;
    this->next_vertex = orig->next_vertex;
    this->owner = orig->owner;
    this->neighbour = orig->neighbour;
  }

  tree_vertex() {
    tree_vertex(NULL, 0);
  }
  std::shared_ptr<tree_vertex> add_next(size_t start, size_t length = 1) {
    if (next_vertex == NULL) {
      next_vertex = std::shared_ptr<tree_vertex>(new tree_vertex(owner, start));
      return next_vertex;
    }
    return next_vertex->add_neighbour(start, length);
  }
  std::shared_ptr<tree_vertex> add_neighbour(size_t start, size_t length = 1) {
    if (owner->compare(this->start_pos, this->_length, start, length)) {
      return std::shared_ptr<tree_vertex<Graph> >(this);
    }
    if (neighbour == NULL) {
      neighbour = std::shared_ptr<tree_vertex>(new tree_vertex<Graph>(owner, start, length));
      return neighbour;
    } 
    return neighbour->add_neighbour(start, length);

  }
  //utility function
  int get_vertex_index(char letter) {
    switch(letter) {
    case 'A':
    case 'a':
      return 0;
    case 'C':
    case 'c':
      return 1;
    case 'G':
    case 'g':
      return 2;
    case 'T':
    case 't':
      return 3;
    }
    return -1; // or should raise some exception instead
  }
};


class SuffixTree {
public:
  bool compare(size_t s1, size_t l1, size_t s2, size_t l2) {
    if (l1 != l2)
      return false;
    for (size_t i = 0; i < l1; i++) {
      if (_str[s1 + i] != _str[s2 + i])
        return false;
    }
    return true;
  }
private:
  std::string _str;
  std::shared_ptr<tree_vertex<SuffixTree> > root;
public:

  
  SuffixTree(std::string & base_str): _str(base_str.c_str()) {
    root = std::shared_ptr<tree_vertex<SuffixTree> > (new tree_vertex<SuffixTree>(this, 0) );
    for (size_t i = 0; i < _str.length(); i++) {
      AddString(i);
    }
  }
protected:
  void AddString(size_t start_index) {
    std::shared_ptr<tree_vertex<SuffixTree> > current_vertex(root);
    for (size_t i = start_index; i < _str.length(); i++) {
      current_vertex = current_vertex->add_next(i);
    }

  }
  
};