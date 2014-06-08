#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

typedef std::vector<std::pair<std::string, std::string> > FastaArray;
/* this struct should contain basic fasta reading functionality*/
struct FastaData{
  std::vector <std::pair<std::string, std::string> > data;
  // first pair element is fasta string label
  // second is fasta string itself

  FastaData() {  }

  bool loadFromFile(const char* filename) {
    std::ifstream input_file(filename);
    if (input_file.fail())
      return false;
    loadFromStream(input_file);
    input_file.close();
    return true;
  }
  
  void insertPair(std::string const & a,std::string const & b) {
    data.push_back(make_pair(a.substr(0), b.substr(0)));
  }

  bool loadFromStream(std::istream& istream) {
    data.clear();
    
    std::string buffer = "";

    while (!istream.eof()) {
      std::getline(istream, buffer);
      buffer.erase(std::remove_if(buffer.begin(), buffer.end(), ::iscntrl), buffer.end());
      processString(buffer);
    }
    return true;
  }

  //the following method somehow tries to read fasta file
  void processString(std::string const& str) {
    if (str.length() == 0)
      return;
    if (str[0] == '>') {
      data.push_back(std::pair<std::string, std::string>(str.substr(1), ""));
    }
    else {
      if (data.size() > 0) {
        data.back().second.append(str);
      }
      else return;
    }
  }

  void debug_print(std::ostream & os) {
    for (std::vector<std::pair<std::string, std::string> >::iterator i = data.begin(); i != data.end(); ++i) {
      os << i->first << ", " << i->second << std::endl;
    }
  }
};