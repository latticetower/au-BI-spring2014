#include <iostream>
#include <fstream>

#include "fasta_data.h"
#include "distance_tools.h"



std::string get_result_string(std::string const& str, std::vector<ArrowDirection> const * path, int n) {
  std::string buffer = "";
  int i = 0;
  for (std::vector<ArrowDirection>::const_reverse_iterator iter = path->rbegin(); iter != path->rend(); ++iter) {
    
    if (*iter == Match) {
      if (i >= (int)str.size())
        break;
      buffer.push_back(str[i]); 
      i++;
      continue;
    }
    if (*iter == Left) {
      if (n == 1) {
        if (i >= (int)str.size())
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
        if (i >= (int)str.size())
          break;
        buffer.push_back(str[i]); i++;
      }
    }
  }
  return buffer;
}

std::string get_result_string2(std::string const & str_a, std::string const & str_b, std::vector<ArrowDirection> const* path, int n) {
  if (str_a.size() < str_b.size())
    return get_result_string2(str_b, str_a, path, n);
  return get_result_string((n == 1 ? str_a : str_b), path, n);
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
  DistanceEstimator distance_estimator(100);
  std::pair<int, std::vector<ArrowDirection>const * > result = distance_estimator.levenshtein_dist(fasta.data[0].second, fasta.data[1].second);
  if (result.first >= 0)
    output_file //<< result.first << std::endl 
    << get_result_string2(fasta.data[0].second, fasta.data[1].second, result.second, 1) << std::endl 
    << get_result_string2(fasta.data[0].second, fasta.data[1].second, result.second, 2);
  else output_file<< "not similar";
  output_file.close();
  return 0;
}