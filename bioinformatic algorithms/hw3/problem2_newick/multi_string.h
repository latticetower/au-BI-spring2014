#pragma once
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>

class MultiString {
  std::vector<std::set<char> > container;
  long score;
public:
  MultiString() {
    container.clear();
    score = 0;
  }

  MultiString(std::string & str) {
    MultiString();
    load_from_string(str);
  }
  void select_sequence() {
    for (std::vector<std::set<char> >::iterator iter = container.begin(); iter != container.end(); ++iter) {
      if (iter->size() > 1) {
        char c = *iter->begin();
        iter->clear();
        iter->insert(c);
      }
    }
  }
  MultiString(MultiString & orig) {
    if (this == &orig)
      return;
    this->container.clear();
    this->score = orig.score;
   
    std::copy(orig.container.begin(), orig.container.end(), std::inserter(this->container, this->container.begin()));
  }

  void load_from_string(std::string const & str) {
    container.resize(str.size());
    for (size_t i = 0; i < str.size(); i++) {
      container[i].insert(str[i]);      
    }
  }
  long getScore() {
    return score;
  }

  void clear() {
    this->container.clear();
  }
  MultiString operator + (MultiString const& op ) const {
    MultiString m;
    m.container.resize(std::max(container.size(), op.container.size()));
    size_t i = 0;
    for ( ; i < std::min(this->container.size(), op.container.size()); i++) {
      std::set<char> temp;
      
      std::set_intersection(this->container[i].begin(), this->container[i].end(), op.container[i].begin(), op.container[i].end(), 
        std::inserter(temp, temp.begin()));
      if (temp.size() > 0) {
        m.container[i].insert(temp.begin(), temp.end());
      }
      else {
        std::set_union(this->container[i].begin(), this->container[i].end(), op.container[i].begin(), op.container[i].end(), 
          std::inserter(temp, temp.begin()));
        m.container[i].insert(temp.begin(), temp.end());
        m.score ++;
      }
    }
    for ( ; i < this->container.size(); i++) {
      m.container[i].insert(this->container[i].begin(), this->container[i].end());
      m.score ++;
    }
    for ( ; i < op.container.size(); i++) {
      m.container[i].insert(op.container[i].begin(), op.container[i].end());
      m.score ++;
    }
    return m;
  }

  MultiString operator & (MultiString const& op ) const {
    MultiString m;
    m.container.resize(std::max(container.size(), op.container.size()));
    size_t i = 0;
    for ( ; i < std::min(this->container.size(), op.container.size()); i++) {
      std::set<char> temp;
      
      std::set_intersection(this->container[i].begin(), this->container[i].end(), op.container[i].begin(), op.container[i].end(), 
        std::inserter(temp, temp.begin()));
      if (temp.size() > 0) {
        m.container[i].insert(temp.begin(), temp.end());
      }
      else {
        m.container[i].insert(this->container[i].begin(), this->container[i].end());
        m.score ++;
      }
    }
    for ( ; i < this->container.size(); i++) {
      m.container[i].insert(this->container[i].begin(), this->container[i].end());
      m.score ++;
    }
    for ( ; i < op.container.size(); i++) {
      //m.container[i].insert(op.container[i].begin(), op.container[i].end());
      //m.score ++;
    }
    return m;
  }
  std::string to_s() {
    std::string buffer = "";
    for (std::vector<std::set<char> >::iterator iter = container.begin(); iter != container.end(); ++iter) {
      if (iter->size() > 1) {
        buffer.push_back('{');
        for (std::set<char>::iterator iter2 = iter->begin(); iter2 != iter->end(); ++iter2) {
          buffer.push_back(*iter2);
        }
        buffer.push_back('}');
      }
      else buffer.push_back(*iter->begin());
    }
    return buffer;
  }
};