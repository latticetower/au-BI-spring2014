#pragma once
#include <string>
#include <algorithm>
/* this file contains helper function for computations */

struct pair_less_comparator {
  bool operator() (std::pair<int, int> const & a, std::pair<int, int> const & b) const
  {
    if (a.first != b.first){
      return a.first < b.first;
    }
    return a.second < b.second;
  }
};


int cost(char a, char b) {
  if (a == b) 
    return 0;
  else return 1;
}

int levenshtein_distance(std::string const& a, std::string const& b) {
  //let's assume that 1st string is longer than second
  if (a.size() < b.size())
    return levenshtein_distance(b, a);
  std::vector<int> editValues(a.size() + 1);
  for (int i = 0; i < a.size(); i ++) {
    editValues[i + 1] = editValues[i] + cost('-', a[i]);
  }
  for (int j = 0; j < b.size(); j ++) {
    int lastV = editValues[0];
    editValues[0] = editValues[0] + cost(b[j], '-');
    for (int i = 0; i < a.size(); i ++) {
      lastV = std::min(editValues[i] + cost('-', a[i]), std::min(editValues[i + 1] + cost(b[j], '-'), lastV + cost(a[i], b[j])));
      std::swap(lastV, editValues[i + 1]);
    }
  }
  return editValues.back();
}