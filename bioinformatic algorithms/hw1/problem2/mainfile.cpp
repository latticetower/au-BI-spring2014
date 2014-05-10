#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "fasta_data.h"

const int GAP_COST = 1;
const int MISMATCH_COST = 1;

enum ArrowDirection{
  Match, 
  Left, 
  Down, 
  Invalid
};// arrow directions for backtracing

inline int match(char a, char b) {
  return (a == b ? 0 : MISMATCH_COST);
}

//updates specified array value based on neighbour values and match/mismatch costs
void set_value(std::vector<int> & holder, int i, int j, int k, std::string const& str_a, std::string const& str_b) {
  if (j + 1 < 0 || j + 1 >= holder.size())
    return;
  int new_val = holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]);
  if (j < holder.size() && j >= 0)
    new_val = std::min(new_val, holder[j] + GAP_COST);
  if (j + 2 < holder.size() && j + 2 >= 0)
    new_val = std::min(new_val, holder[j + 2] + GAP_COST);

  //std::min(std::min(holder[j], holder[j + 2]) + GAP_COST, holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]));
  holder[j + 1] = new_val;  
}

//resets previously updated value (method reverses cell value modified by set_value)
ArrowDirection reset_value(std::vector<int> & holder, int i, int j, int k, std::string const& str_a, std::string const & str_b) {
  if (j + 1 < 0 || j + 1 >= holder.size())
    return Invalid;

  int preffered_index = j + 1;
  if (j + 2 < 0 || j + 2 >= holder.size()) {
    preffered_index = j;
  }
  else {
    if (j < 0 || j >= holder.size()) {
      preffered_index = j + 2;
    } else {
      preffered_index = (holder[j + 2] < holder[j] ? j + 2 : j);
    }
  }
  ArrowDirection direction = (preffered_index == j ? Down : Left);
  
  if (holder[j + 1] - match(str_a[i], str_b[j - k + i + 1]) <= holder[preffered_index] && holder[j + 1] >= match(str_a[i], str_b[j - k + i + 1])) {
    holder[j + 1] = holder[j + 1] - match(str_a[i], str_b[j - k + i + 1]);
    direction = Match;
  }
  return direction;
}

std::pair<int, std::vector<ArrowDirection> > levenshtein_dist(std::string & str_a, std::string & str_b, int k) {
  if (str_a.size() < str_b.size())
    return levenshtein_dist(str_b, str_a, k);
  std::vector<int> holder ;
  holder.resize(2*k + 1, 0);
  //initialization
  holder[k] = 0;
  int min_value = 0, min_position = 0;
  for (int i = 0; i < k; i++) {
    holder[k - i - 1] = holder[k - i] + GAP_COST;
    holder[k + i + 1] = holder[k + i] + GAP_COST;
  }

  std::vector<ArrowDirection> backtracing_path;

  for (int i = 0; i < (int)str_a.size(); i++) {
    int j = k - std::min(i + 1, k);
    if (i < k ) {
      min_value = holder[k - i - 1];
      min_position = k - i - 1;
    }
    else {//i>=k
      set_value(holder, i, -1, k, str_a, str_b);
      //holder[0] = std::min(holder[0] + match(str_a[i], str_b[i - k]), holder[1] + GAP_COST);
      min_value = holder[0];
      min_position = 0;
    }
  
    for ( ; j < std::min(2*k - 1, (int)str_b.size() + k - i - 1); j ++) {
      set_value(holder, i, j, k, str_a, str_b);
      
      //holder[j + 1] = std::min(std::min(holder[j], holder[j + 2]) + GAP_COST, holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]));
      if (holder[j + 1] < min_value) {
        min_value = holder[j + 1];
        min_position = j + 1;
      }
    }
    
    if (min_value >= k)
      return std::make_pair(-1, backtracing_path);
  }
  min_position = std::min(2*k, (int)(str_b.size() + k - str_a.size() - 1));
  if (min_value >= k)
      return std::make_pair(-1, backtracing_path);
  ArrowDirection direction;
  //backtracing begins here
  int n = (int)str_a.size();
  for (int i = n - 1; i >= 0; i --) {
    int j0 = k - std::min(i + 1, k);
    
    int j = std::min(2*k - 1, (int)str_b.size() + k - i - 1) - 1;
    for ( ; j >= j0; j --) {
      direction = reset_value(holder, i, j, k, str_a, str_b);
      if (j + 1 == min_position) {
        if (direction == Left) {
          min_position = j+2;
        }
        if (direction == Down) {
          min_position = j;
        }
        backtracing_path.push_back(direction);
      }
    }
    if (i < k ) {
      //min_value = holder[k - i - 1];
      //min_position = k - i - 1;
    }
    else {//i>=k
      direction = reset_value(holder, i, -1, k, str_a, str_b);
      if (0 == min_position) {
        if (direction == Left) {
          min_position = 1;
        }
        backtracing_path.push_back(direction);
      }
      //holder[0] = std::min(holder[0] + match(str_a[i], str_b[i - k]), holder[1] + GAP_COST);
      //min_value = holder[0];
      //min_position = 0;
    }
    
  }

  //backtracing ends here

  return std::make_pair(min_value, backtracing_path);
  
}
std::string get_result_string(std::string& str, std::vector<ArrowDirection> & path, int n) {
  std::string buffer = "";
  int i = 0;
  for (std::vector<ArrowDirection>::reverse_iterator iter = path.rbegin(); iter != path.rend(); ++iter) {
    
    if (*iter == Match) {
      if (i >= str.size())
      break;
      buffer.push_back(str[i]); 
      i++;
      continue;
    }
    if (*iter == Left) {
      if (n == 1) {
        if (i >= str.size())
      break;
        buffer.push_back(str[i]); i++;
      }
      else {
        buffer.push_back('-');
      }
    }
    if (*iter == Down) {
      if (n == 1) {
        buffer.push_back('-');
      }
      else {
        if (i >= str.size())
      break;
        buffer.push_back(str[i]); i++;
      }
    }
  }
  return buffer;
}

//tests
bool test1(int i1, int i2, int i3) {
  std::vector<int> holder;
  holder.resize(3);
  //test1
  holder[0] = i1;
  holder[1] = i2;
  holder[2] = i3;
  set_value(holder, 0, 0, 1, "aaaa", "aaaa");
  ArrowDirection direction = reset_value(holder, 0, 0, 1, "aaaa", "aaaa");
  return (holder[0] == i1) && (holder[1] == i2) && (holder[2] == i3) && (direction == Match);
}

bool test2(int i1, int i2, int i3) {
  std::vector<int> holder;
  holder.resize(3);
  //test1
  holder[0] = i1;
  holder[1] = i2;
  holder[2] = i3;
  set_value(holder, 0, 0, 1, "aaaa", "aaaa");
  ArrowDirection direction = reset_value(holder, 0, 0, 1, "aaaa", "aaaa");
  return (holder[0] == i1) && (holder[1] == i2) && (holder[2] == i3);
}

int main() {
  if (!test1(1, 1, 1))
    return 1;
  if (!test1(1, 2, 3))
    return 3;
  if (!test1(2, 1, 2))
    return 4;
  if (!test1(2, 1, 3))
    return 5;
  return 0;
  
}
int main1(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "Please, provide input file name as a first parameter, and output file name as a second parameter" << std::endl;
    return 0;
  }
  FastaData fasta;
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  fasta.loadFromStream(inputStream);
  inputStream.close();
  
  std::ofstream output_file(argv[2]);
  
  std::pair<int, std::vector<ArrowDirection> > result = levenshtein_dist(fasta.data[0].second, fasta.data[1].second, 10);
  if (result.first >= 0)
    output_file << result.first << std::endl 
    << get_result_string(fasta.data[0].second, result.second, 1) << std::endl 
    << get_result_string(fasta.data[1].second, result.second, 2);
  else output_file<< "not similar";
  output_file.close();
  return 0;
}