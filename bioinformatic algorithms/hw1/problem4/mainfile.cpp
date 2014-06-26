#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "fasta_data.h"
#include "hashing.h"


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
  if (fasta.data[0].second.size() < 2) {
    output_file << "no substring with reverse complement substring found";
    output_file.close();
    return 0;
  }
  RevComplHasher hasher;
  hasher.PrepareHash(fasta.data[0].second);
  std::pair<size_t, size_t> result = hasher.get_max_substr(0, fasta.data[0].second.size()/2 + 1, -1);
  if (result.second == 0) {
    output_file << "no substring with reverse complement substring found";
  }
  else
    output_file << fasta.data[0].second.substr(result.first, result.second);
  output_file.close();
  return 0;
}