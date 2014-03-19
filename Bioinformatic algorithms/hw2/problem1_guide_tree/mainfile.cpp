#include <iostream>
#include "fasta_data.h"


int main(int argc, char**argv) {
  FastaData fasta;
  if (argc < 2) {
    std::cout << "Please, provide file name as a parameter" << std::endl;
    return 0;
  }
  if (!fasta.loadFromFile(argv[1])) {
    std::cout << "Please, provide _valid_ file name as a parameter" << std::endl;
    return 0;
  }
  fasta.debug_print(std::cout);
  return 0;

}