#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "fasta_data.h"

const int GAP_COST = 1;
const int MISMATCH_COST = 1;

inline int match(char a, char b) {
  return (a == b ? 0 : MISMATCH_COST);
}

int levenshtein_dist(std::string & str_a, std::string & str_b, int k) {
  if (str_a.size() <str_b.size())
    return levenshtein_dist(str_b, str_a, k);
  std::vector<int > holder ;
  holder.resize(2*k + 1, 0);
  //начало
  holder[k] = 0;
  int min_value = 0;
  for (int i = 0; i < k; i++) {
    holder[k - i - 1] = holder[k - i] + GAP_COST;
    holder[k + i + 1] = holder[k + i] + GAP_COST;
  }

  for (int i = 0; i < (int)str_a.size(); i++) {
    int j = k - std::min(i + 1, k);
    if (i < k ){
      min_value = holder[k - i - 1];
    }
    else {//i>=k
      holder[0] = std::min(holder[0] + match(str_a[i], str_b[i - k]), holder[1] + GAP_COST);
      min_value = holder[0];
    }
  
    for ( ; j < std::min(2*k - 2, (int)str_b.size() + k - i - 2); j ++) {
      holder[j + 1] = std::min(std::min(holder[j], holder[j + 2]) + GAP_COST, holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]));
      if (holder[j + 1] < min_value)
        min_value = holder[j + 1];
    }
  
    holder[j + 1] = std::min(std::min(holder[j], holder[j + 2]) + GAP_COST, holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]));
    if (holder[j + 1] < min_value)
        min_value = holder[j + 1];

    if (min_value >= k)
      return -1;
  }
  
  if (min_value >= k)
      return -1;
  return min_value;
  
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
  
  int result = levenshtein_dist(fasta.data[0].second, fasta.data[1].second, 100);
  if (result >= 0)
    output_file << result;
  else output_file<< "not similar";
  output_file.close();
  return 0;
}