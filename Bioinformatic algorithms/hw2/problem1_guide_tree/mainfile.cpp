#include <iostream>
#include "fasta_data.h"
#include "computations.h"
#include "guide_tree.h"
#include "printer.h"


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
  //fasta.debug_print(std::cout);
  //std::cout << levenshtein_distance("aaa", "aattt");
  GuideTree guide_tree(&fasta);
  guide_tree.Build();
  Printer printer(&fasta);
  guide_tree.acceptVisitor(&printer);
  return 0;

}