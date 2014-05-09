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
void set_value(std::vector<int> & holder, int i, int j, int k, std::string & str_a, std::string & str_b) {
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
ArrowDirection reset_value(std::vector<int> & holder, int i, int j, int k, std::string & str_a, std::string & str_b) {
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
  
  if (holder[j + 1] - match(str_a[i], str_b[j - k + i + 1]) <= holder[preffered_index]) {
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
//    set_value(holder, i, j, k, str_a, str_b);
      
  //
    //holder[j + 1] = std::min(std::min(holder[j], holder[j + 2]) + GAP_COST, holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]));
    //if (holder[j + 1] < min_value) {
    //  min_value = holder[j + 1];
    //  min_position = j + 1;
    //}
    
    if (min_value >= k)
      return std::make_pair(-1, backtracing_path);
  }
  
  if (min_value >= k)
      return std::make_pair(-1, backtracing_path);

  //backtracing begins here

  //middle part
  /*int n = (int) str_a.size();
  for (int i = n - 1; i >= k; i ++) {
    int j = std::min(2*k - 1, (int)(str_b.size() - i - 1 + k));
    holder[j + 1] = std::min(holder[j + 1] + match(str_a[i], str_b[i - k + j]), holder[j] + GAP_COST);
    j--;
    for (; j >= 0; j--) {
      holder[j + 1] = std::min( std::min(holder[j], holder[j + 2]) + GAP_COST, holder[j + 1] + match(str_a[i], str_b[i - k + j + 1]));  
      if (holder[j + 1] < min_value) {
        min_value = holder[j + 1];
        min_position = j + 1;        
        }
    }
    holder[0] = std::min(holder[0] + match(str_a[i], str_b[i - k]), 
                holder[1] + GAP_COST);
    
    if (holder[j + 1] < min_value) {
        min_value = holder[j + 1];
        min_position = j + 1;
        }
    
    if (min_value >= k)
      return -1;
  }
  //начало
  holder[k] = 0;
  int min_value = 0;
  for (int i = 0; i < k; i++) {
    holder[k - i - 1] = holder[k - i] + GAP_COST;
    holder[k + i + 1] = holder[k + i] + GAP_COST;
  }
  for (int i = 0; i < std::min(k, (int)str_a.size()); i++) {
    int j = k - i;
    min_value = holder[k - i - 1];
    for ( ; j < std::min(2*k - 1, (int)str_b.size() + k - i - 1); j ++) {
      holder[j] = std::min(std::min(holder[j - 1], holder[j + 1]) + GAP_COST, holder[j] + match(str_a[i], str_b[j - k + i]));
      if (holder[j] < min_value)
        min_value = holder[j];
    }
    holder[j] = std::min(holder[j - 1] + GAP_COST, holder[j] + match(str_a[i], str_b[j - k + i]));
    if (holder[j] < min_value)
        min_value = holder[j];

    if (min_value >= k)
      return -1;
  }
  */
  //backtracing ends here

  return std::make_pair(min_value, backtracing_path);
  
}

int main(int argc, char** argv) {
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
  
  int result = levenshtein_dist(fasta.data[0].second, fasta.data[1].second, 10).first;
  if (result >= 0)
    output_file << result;
  else output_file<< "not similar";
  output_file.close();
  return 0;
}