#include <iostream>
#include <fstream>
#include "suffix_tree.h"
#include "newick_writer.h"
#include "fasta_data.h"
#include "bwt_builder.h"

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cout << "Please, provide input file name as a first parameter, and output file name as a second parameter" << std::endl;
    return 0;
  }
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  FastaData text_fasta;
  text_fasta.loadFromStream(inputStream);
  inputStream.close();

  std::ifstream inputStream2(argv[2]);
  if (inputStream2.fail()) {
    std::cout << "Please, provide _valid_ file name as second parameter" << std::endl;
    return 0;
  }
  FastaData strings_fasta;
  strings_fasta.loadFromStream(inputStream2);
  inputStream2.close();
  //now we have 2 fasta objects with given text and set of given substrings

  SuffixTree tree(text_fasta.data[0].second);
  BWTBuilder bwt_builder;
  tree.acceptVisitor(&bwt_builder);
  NewickWriter newick;
  tree.acceptVisitor(&newick);
  std::vector<long> result = bwt_builder.getBWT();
  std::ofstream output_file(argv[3]);
  output_file << ">tree processed as" << std::endl << newick.getResult() << std::endl;
    
  for (std::vector<long>::iterator iter = result.begin(); iter!= result.end(); ++iter) {
    output_file << *iter << "\t";
  }
  output_file.close();
  //std::ofstream of("data.txt");
  //NewickWriter writer;
  //tree.acceptVisitor(&writer);
  //of << writer.getResult();
  //of.close();
  return 0;
}