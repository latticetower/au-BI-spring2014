#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "fasta_data.h"
#include "hashing.h"

size_t start_i = 0, start_j = 0, length = 0;

void find_sequences(std::string const& str, Hasher& hasher) {
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
  find_sequences(fasta.data[0].second, hasher);
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