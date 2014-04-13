#pragma once
#include <memory>
#include <set>

//ititially we assume that all vertices describe the same string substrings
template <class Graph>
class tree_vertex {
private:
  int get_id() {
    return (int)this;
  }
  std::shared_ptr<tree_vertex> next_vertex; // really im not sure now what to store
  std::shared_ptr<tree_vertex> neighbour; //next neighbour for current vertex
  size_t length;
  Graph * owner;

  std::set<size_t> start_positions;
public:

  void set_parent(Graph * g) {
    owner = g;
  }
  
  ~tree_vertex() {
    next_vertex.reset();
    neighbour.reset();
  }
  tree_vertex(Graph * g, size_t start, size_t _length = 1): next_vertex(NULL), neighbour(NULL), length(_length), owner(g) { 
    start_positions.clear();
    start_positions.insert(start);
  }

  tree_vertex(tree_vertex & orig) {
    this->start_positions.clear();//TODO: check this
    for (std::set<size_t> ::iterator iter = orig->start_positions.begin(); iter != orig->start_positions.end(); ++iter) {
     this->start_positions.insert(*iter);
    }
    //this->start_pos = orig->start_pos;
    this->length = orig->length;
    this->next_vertex = orig->next_vertex;
    this->owner = orig->owner;
    this->neighbour = orig->neighbour;
  }

  tree_vertex() {
    tree_vertex(NULL, 0);
  }

  tree_vertex<Graph>* add_next(size_t start, size_t length = 1) {
    if (next_vertex == NULL) {
      next_vertex = std::shared_ptr<tree_vertex<Graph> >(new tree_vertex<Graph>(owner, start, length));
      return next_vertex.get();
    }
    return next_vertex->add_neighbour(start, length);

  }

  tree_vertex<Graph> * add_neighbour(size_t start, size_t _length = 1) {
    if (owner->compare(*start_positions.begin(), length, start, _length)) {
      start_positions.insert(start);
      return this;
    }
    if (neighbour == NULL) {
      neighbour = std::shared_ptr<tree_vertex<Graph> >(new tree_vertex<Graph>(owner, start, length));
      return neighbour.get();
    } 
    return neighbour->add_neighbour(start, length);
  }
  bool operator == (tree_vertex<Graph> const& obj) {
    return (owner == obj.owner) && 
      (start_positions.size() == obj.start_positions.size())  ;
  }

  void simplify() {
    if (next_vertex == NULL)
      return;
    next_vertex.get()->simplify();
    if (*this == *next_vertex.get() && next_vertex->neighbour == NULL) {
      this->length += next_vertex.get()->length;
      next_vertex = next_vertex.get()->next_vertex;
    }
    if (neighbour != NULL)
      neighbour.get()->simplify();

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

  
  SuffixTree(std::string & base_str) {
    _str = std::string(base_str.c_str());
    root = std::shared_ptr<tree_vertex<SuffixTree> > (new tree_vertex<SuffixTree>(this, 0) );
    for (size_t i = 0; i < _str.length(); i++) {
      AddString(i);
    }
  }
  
  void Simplify() {
    root->simplify();
  }

protected:
  void AddString(size_t start_index) {
    tree_vertex<SuffixTree>* current_vertex = root.get();
    for (size_t i = start_index; i < _str.length(); i++) {
      current_vertex = current_vertex->add_next(i);
    }

  }

  
};