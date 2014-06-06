#pragma once
#include <memory>
#include <set>
#include <algorithm>
#include <vector>
#include <sstream>
#include "ivisitor.h"

bool compare_pairs(std::pair<size_t, size_t> a, std::pair<size_t, size_t> b) {
    return a.second > b.second;
 }


//ititially we assume that all vertices describe the same string substrings
template <class Graph>
class tree_vertex {
private:
  int get_id() {
    return (int) this;
  }
  size_t length;
  Graph * owner;
  char value;//'A', 'C', 'G' or 'T'. root is denoted by letter 'N'
public:

  long string_length() const {
    return owner->max_length();
  }
  long index;

  bool is_leaf;//indicates if current element is last in suffix
  struct classcomp {
    bool operator() (const std::shared_ptr<tree_vertex<Graph> > & lhs, const std::shared_ptr<tree_vertex<Graph> > & rhs) const {
      return lhs->value < rhs->value;
    }
  };

  std::set<std::shared_ptr<tree_vertex >, classcomp> next_vertices; // really im not sure now what to store
  
  
  void set_parent(Graph * g) {
    owner = g;
  }
  
  ~tree_vertex() {
    next_vertices.clear();
  }
  tree_vertex(Graph * g, size_t start, char _value, size_t _length = 1): length(_length), owner(g) { 
    next_vertices.clear();
    value = _value;
    is_leaf = false;
    index = 0;
    //start_positions.insert(start);
  }

  tree_vertex(tree_vertex & orig) {
    this->start_positions.clear();//TODO: check this
    for (std::set<size_t> ::iterator iter = orig.start_positions.begin(); iter != orig.start_positions.end(); ++iter) {
     this->start_positions.insert(*iter);
    }
    this->is_leaf = orig.is_leaf;
    this->value = orig.value;
    //this->start_pos = orig->start_pos;
    this->length = orig->length;
    this->next_vertex = orig.next_vertex;
    this->index = orig.index;
    this->owner = orig.owner;
    this->neighbour = orig.neighbour;
  }

  tree_vertex() {
    tree_vertex(NULL, 0);
    index = 0;
    is_leaf = false;
  }
  
  tree_vertex<Graph>* add_next(size_t start, char value, size_t length = 1) {
    std::shared_ptr<tree_vertex<Graph> > next_vertex = std::shared_ptr<tree_vertex<Graph> >(new tree_vertex<Graph>(owner, start, value, length));
    std::set<std::shared_ptr<tree_vertex<Graph> >, tree_vertex<Graph>::classcomp>::iterator search_result = next_vertices.find(next_vertex);  
    if (search_result == next_vertices.end()) {
      next_vertices.insert(next_vertex);
      return next_vertex.get();
    }
    else {
      return (*search_result).get();
    }
  }
  /*
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
  }*/
  bool operator == (tree_vertex<Graph> const& obj) {
    return (owner == obj.owner) && 
      (start_positions.size() == obj.start_positions.size())  ;
  }

  /*
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
  }*/
  

  /*
  std::pair<size_t, size_t> traverse(std::set<size_t>& prefixes, size_t const & prefix_pos, size_t acc) {
    if (this->start_positions.size() < MIN_REPEATS_COUNT) {
      if (this->start_positions.find(prefix_pos - 1) != this->start_positions.end() && 
        this->start_positions.find(prefix_pos - 1 + acc) != this->start_positions.end() &&
          prefixes.find(prefix_pos - acc) != prefixes.end()) {// == *iter + length) {
            return std::make_pair(prefix_pos, acc );
      }
      else
        return std::make_pair(0, 0);
    }
    std::vector<std::pair<size_t, size_t> > candidates;
    if (neighbour != NULL) {
      if (this->start_positions.find(prefix_pos) != this->start_positions.end()) {

      } else {
        if (this->neighbour->start_positions.size() < MIN_REPEATS_COUNT) {
          if (this->start_positions.find(prefix_pos - 1) != this->start_positions.end() && 
        this->start_positions.find(prefix_pos - 1 + acc) != this->start_positions.end() &&
          prefixes.find(prefix_pos - acc) != prefixes.end()) {// == *iter + length) {
            candidates.push_back(std::make_pair(prefix_pos, acc));
          }
        }
        candidates.push_back(this->neighbour->traverse(prefixes, prefix_pos, acc));
      }
    }*/

    //1. проходим по всем таким участкам: если следующий за ними элемент находится в списке префиксов, то это наш кандидат
    //for (std::set<size_t>::iterator iter = this->start_positions.begin(); iter != start_positions.end(); ++iter) {
  /*    if (this->start_positions.find(prefix_pos - length) != this->start_positions.end() && 
        this->start_positions.find(prefix_pos + acc) != this->start_positions.end() &&
          prefixes.find(prefix_pos - acc -length) != prefixes.end()) {// == *iter + length) {
        candidates.push_back(std::make_pair(prefix_pos, acc + length));
      }
      candidates.push_back(this->next_vertex->traverse(prefixes, prefix_pos, acc + length));
    
    //}
    std::sort(candidates.begin(), candidates.end(), compare_pairs);
    if (candidates.size() == 0)
      return std::make_pair(0, 0);
    return candidates.at(0);
  }
  */
  /*
  std::pair<size_t, size_t> traverse() {
    std::pair<size_t,size_t> r1 = std::make_pair(0, 0);
    std::pair<size_t, size_t> r2 ;
    if (next_vertex != NULL) {
      for (std::set<size_t>::iterator iter = next_vertex->start_positions.begin(); iter != next_vertex->start_positions.end(); ++ iter) {
        r2 = next_vertex->traverse(next_vertex->start_positions, *iter, 0);
        if (r2.second > r1.second)
          r1 = r2;
      }
    }
    tree_vertex<Graph> * current_neighbour = next_vertex->neighbour.get();
    while (current_neighbour != NULL) {
      for (std::set<size_t>::iterator iter = current_neighbour->start_positions.begin(); iter != current_neighbour->start_positions.end(); ++ iter) {
        r2 = current_neighbour->traverse(current_neighbour->start_positions, *iter, 0);
        if (r2.second > r1.second)
          r1 = r2;
      }
      current_neighbour = current_neighbour->neighbour.get();
    }
    return r1;
  }*/

  void acceptVisitor(IVisitor * visitor) {
    return visitor->visit(this);
  }
  /*
  tree_vertex<Graph> * getNext() {
    return next_vertex.get();
  }
  */
  /*
  tree_vertex<Graph> * getNeighbour() {
    return neighbour.get();
  }
  */
  /*
  std::string to_s() {
    std::stringstream ss;
    std::set<size_t>::iterator iter = this->start_positions.begin();
    ss << "leaf[ (" << *iter; 
    ++ iter;
    for ( ; iter != start_positions.end(); ++iter) {
      ss << "," << *iter;
    }
    ss <<") - " << this->length << "] ";
    //if (this->next_vertex != NULL)
    //  ss << next_vertex->to_s() << ";" << std::endl;
    //else ss << "[]" << std::endl;

    return ss.str();
  }
  */
  
  char get_value() {
    return value;
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
  long max_length() const {
    return _str.length();
  }
  tree_vertex<SuffixTree>* getRoot() {
    return root.get();
  }
  
  SuffixTree(std::string & base_str) {
    _str = std::string(base_str.c_str());
    _str.push_back('z');
    root = std::shared_ptr<tree_vertex<SuffixTree> > (new tree_vertex<SuffixTree>(this, 'N', 0) );
    for (size_t i = 0; i < _str.length(); i++) {
      AddString(i);
    }
  }
  /*
  void Simplify() {
    root->simplify();
  }
  */
  /*
  std::pair<size_t, size_t> Traverse() {
    if (root == NULL)
      return std::make_pair(0, 0);
    return root->traverse();
  }
  */
protected:
  void AddString(size_t start_index) {
    tree_vertex<SuffixTree>* current_vertex = root.get();
    for (size_t i = start_index; i < _str.length(); i++) {
      current_vertex = current_vertex->add_next(i, _str[i]);
    }
    current_vertex->index = start_index;
    current_vertex->is_leaf = true;
  }
public:
  void acceptVisitor(IVisitor * visitor) {
    return visitor->visit(this);
  }  
 

};