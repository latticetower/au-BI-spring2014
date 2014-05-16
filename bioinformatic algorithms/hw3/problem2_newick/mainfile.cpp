#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include "binary_rooted_tree.h"
#include "fasta_data.h"

int main(int argc, char** argv){
  //let's assume that 
  // 1. all vertices are connected
  // 2. adjacency list describes tree
  // 3. graph is undirected
  if (argc < 3) {
    std::cout << "Please, provide input file name as a first parameter, and output file name as a second parameter" << std::endl;
    return 0;
  }
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  BinaryRootedTree tree;
  tree.loadStructure(inputStream);
  FastaData fasta;
  fasta.loadFromStream(inputStream);
  inputStream.close();
  
  tree.loadSequencesFromFasta(&fasta);
  tree.constructSets();
  std::ofstream output_file(argv[2]);
  output_file << tree.getScore() << std::endl;
  tree.traverseTopDown();
  FastaData result = tree.getInnerNodeSequences();
  for (std::vector<std::pair<std::string, std::string> > ::iterator iter = result.data.begin(); iter != result.data.end(); ++iter) {
    output_file << ">" << iter->first << std::endl << iter->second << std::endl;
  }
  output_file.close();
  return 0;
}