#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "fasta_data.h"
#include "scoring.h"
#include "string_aligner_simplest.h"



int main(int argc, char** argv) {
  if (argc < 4) {
    std::cout << "Please, provide input file name as a first parameter, scoring matrix file name as a second parameter, and output file name as a third parameter" << std::endl;
    return 0;
  }
  // 1. read fasta strings:
  FastaData fasta;
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  fasta.loadFromStream(inputStream);
  inputStream.close();
  // 2. read scoring matrix:
  Scoring scoring;
  std::ifstream inputStream2(argv[2]);
  if (inputStream2.fail()) {
    std::cout << "Please, provide _valid_ file name as second parameter" << std::endl;
    return 0;
  }
  scoring.loadFromStream(inputStream2);
  inputStream2.close();
  std::vector<std::string> data;
  for (int i = 0; i < (int)fasta.data.size(); i++) {
    data.push_back(fasta.data[i].second);
  }
  Simple4dAligner aligner(&data, &scoring);
  aligner.Align();
  std::vector<std::string> str_array = aligner.ReconstructAlignment();
  std::ofstream output_file(argv[3]);
 
  for (int i = 0; i < 4; i ++) {
    output_file << str_array[i] << std::endl;
  }
    
  output_file.close();
  return 0;
}