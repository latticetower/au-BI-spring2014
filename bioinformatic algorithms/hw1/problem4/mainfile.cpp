#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "fasta_data.h"


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
 
  
  
  output_file.close();
  return 0;
}