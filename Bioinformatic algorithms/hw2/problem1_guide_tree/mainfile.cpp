#include <iostream>
#include "fasta_data.h"
#include "computations.h"
#include "guide_tree.h"
#include "printer.h"
#include "newick_writer.h"


int main(int argc, char**argv) {
  FastaData fasta;
  if (argc < 2) {
    std::cout << "Please, provide input file name as a first parameter, and (optionally) output file name as a second parameter" << std::endl;
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
  //Printer printer(&fasta);
  //guide_tree.acceptVisitor(&printer);
  NewickWriter newick(&guide_tree, &fasta);
  guide_tree.acceptVisitor(&newick);
  if (argc < 3) {
    std::cout << newick.getResult();
  }
  else {
    std::ofstream output_file(argv[2]);
    output_file << newick.getResult();
    output_file.close();
  }
  return 0;

}