#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "fasta_data.h"
#include "rna_tools.h"


int main(int argc, char** argv) {
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
  RNAPredictor nussinov_predictor(4, 0.1);
  nussinov_predictor.processSeq(fasta.data[0].second);
  std::ofstream os("maxtrix.txt");
  nussinov_predictor.debug_print(os);
  os.close();
  std::vector<std::pair<ResultElType, ResultElType> > result = nussinov_predictor.get_result(0, fasta.data[0].second.size() - 1, fasta.data[0].second);

  std::ofstream output_file(argv[2]);
  if (result.size() == 0) {
    output_file << "no pairs of complimentary sequences were found";
  }
  for (size_t i = 0; i < result.size(); i++) {
    output_file << result[i].first.first << "," << result[i].first.second << "-" << result[i].second.first << "," << result[i].second.second << std::endl;
  }
  output_file.close();
  return 0;
}