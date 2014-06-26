#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "fasta_data.h"
#include "hashing.h"

size_t start_i = 0, start_j = 0, length = 0;

size_t check_length(std::string const & str, size_t i, size_t j, size_t max_length) {
  if (max_length == 0)
    return 0;
  int counter = 0;
  for (size_t k = 0; k < max_length; k++) {
    if (str[i + k] != str[j + k]) {
      if (counter > 0) {
        return k;
      }
      else {
        counter++;//detect mismatch, go next
      }
    }
    if (i + k + 1 >= j) {
      return k + 1;
    }
  }
  return max_length;
}

//returns current length. if greater then specified - finds max length
// if not - returns zero
size_t check_length2(std::string const & str, size_t i, size_t j, size_t max_length) {
  if (i > j)
    return check_length2(str, j, i, max_length);
  size_t current_length = max_length;
  if (j + current_length > str.size())
    return 0;//there is no way to improve max_length
  if (i + current_length >= j)
    return 0;// there is still no way to improve max_length

  if (str[i + max_length] == str[j + max_length]) {
    //this means that current str might have greater length than current best string
    current_length = check_length(str, i, j, max_length + 1);
  }
  else {
    if (j + max_length + 1 >= str.size() || i + max_length + 1 >= j) {
      return 0;// 
    }
    if (str[i + max_length + 1] == str[j + max_length + 1]) {
      current_length = check_length(str, i, j, max_length + 1);
    }
  }
  return current_length;
}

void find_sequences(std::string const &str) {
  if (str.size() <= 1) {
    return; //do not update values for positions
  }
  start_i = 0; start_j = 1;
  length = 0;
  if (str.size() <= 3) {
    
    return;
  }
  
  std::vector<size_t> candidate_positions;
  for (size_t i = 0; i < std::min(str.size() - 2*length + 1, str.size()); i++) {
    for (size_t j = i + length + 1; j < std::min(str.size() - length + 1, str.size()); j++) {
      size_t len = check_length2(str, i, j, length);
      if (len > length) {
        length = len;
        start_i = i;
        start_j = j;
        candidate_positions.clear();
      }
      else {
        if (len == length) {
          candidate_positions.push_back(j);
        }
      }
    }
   
  }
  
}

void find_sequences2(std::string const& str, Hasher& hasher) {
  start_i = 0; start_j = 1;
  length = 1;
  if (str.size() < 2) {
    length = 0;
    return;
  }
  
  //std::vector<size_t> candidate_positions;
  for (size_t i = 0; i < std::min(str.size() - 2*length + 1, str.size()); i++) {
    //size_t divider = i + length;
    //size_t length0 = length; // need this to correctly iterate through inner cycle
    //following string was used for optimizations in iteration process:
    //for (size_t j = hasher.get_next(i, divider, 0, 1); j < std::min(str.size() - length0 + 1, str.size()); j = hasher.get_next(i, divider, j, length0)) {
    for (size_t j = i + length; j < std::min(str.size() - length + 1, str.size()); j ++) {
      size_t len = hasher.CheckLength(i, j, length);
      if (len > length) {
        length = len;
        start_i = i;
        start_j = j;
      }
    }
    //
  }
  
}




int main(int argc, char** argv){
  if (argc < 3) {
    std::cout << "Please, provide input file name as a first parameter, and output file name as a second parameter" << std::endl;
    return 0;
  }
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  FastaData fasta;
  fasta.loadFromStream(inputStream);
  inputStream.close();

  std::ofstream output_file(argv[2]);
  Hasher hasher;
  hasher.PrepareHash(fasta.data[0].second);
  find_sequences2(fasta.data[0].second, hasher);
  if (length == 0) {
    output_file << "No data, no sequences.\n";
  }
  else {
    output_file << ">repeat1\n" <<fasta.data[0].second.substr(start_i, length) << std::endl 
      << ">repeat2\n" << fasta.data[0].second.substr(start_j, length) << std::endl;
  }

  
  output_file.close();
  return 0;
}