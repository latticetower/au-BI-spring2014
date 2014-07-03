#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "fasta_data.h"
#include "energies_info.h"
#include "rna_tools.h"


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
  EnergiesInfo ei;
  ei.LoadFromFile("energies.txt");
  RNAEnergiesPredictor zuker_predictor(ei);
  std::ofstream g("debug_info.txt");
  
  zuker_predictor.ProcessSeq(fasta.data[0].second);
  std::cout << "opt energy is " << zuker_predictor.getEnergy() << std::endl;
  zuker_predictor.debug_print_matrices(g);//need some debug info to ensure all is fine
  g.close();
  std::ofstream output_file(argv[2]);
  if (fasta.data[0].second.size() < 1) {
    output_file << "sequence size is invalid";
    return 0;
  }
  std::vector<std::pair<ResultElType, ResultElType> > result = zuker_predictor.get_result(0, fasta.data[0].second.size() - 1, fasta.data[0].second);

  
  if (result.size() == 0) {
    output_file << "no pairs of complimentary sequences were found";
  }
  for (size_t i = 0; i < result.size(); i++) {
    output_file << result[i].first.first << "," << result[i].first.second << "-" << result[i].second.first << "," << result[i].second.second << std::endl;
  }
  
  output_file.close();
  return 0;
}
