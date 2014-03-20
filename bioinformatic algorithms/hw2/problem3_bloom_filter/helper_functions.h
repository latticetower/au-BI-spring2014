#pragma once
#include<set>


//the following comparator just in case - i don't remember if strings are compared lexicography or by pointer
struct string_less_comparator {
  bool operator() (std::string const & a, std::string const & b) const
  {
    return a.compare(b) < 0;
  }
};

typedef std::set<std::string, string_less_comparator> StringContainer;

