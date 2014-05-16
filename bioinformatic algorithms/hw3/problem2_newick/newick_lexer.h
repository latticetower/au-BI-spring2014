#pragma once
#include <string>
#include <vector>

enum Token{LBRACKET, RBRACKET, NAME, END};

class NewickLexer {
  std::vector<std::pair<Token, std::string> > data;
  long current_element;
public:
  NewickLexer() {
    data.clear();
    current_element = 0;
  }

  Token getToken(std::string& a) {
    if (a == "(")
      return LBRACKET;
    if (a == ")")
      return RBRACKET;
    if (a == ";")
      return END;
    return NAME;
  }

  void Build(std::string const & str) {
    data.clear();
    current_element = 0;
    std::string buffer = "";
    for (int i = 0; i < (int)str.length(); i++) {
      if (str[i] == ')' || str[i] == ',' || str[i] == ';'|| str[i] == '(') {
        if (buffer.size() > 0) {
          data.push_back(make_pair(getToken(buffer), buffer));
        }
        if (str[i] != ',') {
            buffer = str[i];
            data.push_back(make_pair(getToken(buffer), buffer));
        }
        buffer = "";
        
      }
      else {
        buffer.push_back(str[i]);
      }
    }
    if (buffer.size() > 0) {
      data.push_back(make_pair(getToken(buffer), buffer));
    }
  }
  std::pair<Token, std::string> peek() {
    return data[current_element];
  }

  std::pair<Token, std::string> next() {
    current_element++;
    return data[current_element - 1];
  }
};