#pragma once
#include<string>
/* this file contains helper function for computations */


int levenshtein_distance(std::string const& a, std::string const& b) {
  //let's assume that 1st string is longer than second
  if (a.size() < b.size())
    return levenshtein_distance(b, a);
}