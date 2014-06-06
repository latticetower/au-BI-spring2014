#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "fasta_data.h"

size_t start_i = 0, start_j = 0, length = 0;

//returns current length. if equal to specific size - some repeat found
size_t check_length(std::string const & str, size_t i, size_t j, size_t max_length) {
  if (max_length == 0)
    return 0;
  int counter = 0;
  for (size_t k = 0; k < max_length; k++) {
    if (str[i + k] != str[j + k]) {
      if (counter > 0) {
        return k;
      }
      else {
        counter++;//detect mismatch, go next
      }
    }
    if (i + k + 1 >= j) {
      return k + 1;
    }
  }
  return max_length;
}

void find_sequences(std::string const &str) {
  if (str.size() <= 1) {
    return; //do not update values for positions
  }
  start_i = 0; start_j = 1;
  length = 1;
  if (str.size() <= 3) {
    
    return;
  }
  long long max_length = str.size() / 2;
  while (max_length > length) {
   for (size_t i = 0; i < str.size() - length; i++) {
      for (size_t j = i + max_length; j < str.size() - max_length + 1; j++) {
        size_t len = check_length(str, i, j, max_length);
        if (len == max_length) {
          length = max_length;
          start_i = i;
          start_j = j;
          return;
        }
        else {
          if (len > length) {
            length = len;
            start_i = i; 
            start_j = j;
          }
        }
      }
    }
  max_length --;

  }
}

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
  find_sequences(fasta.data[0].second);
  if (length == 0) {
    output_file << "No data, no sequences.\n";
  }
  else {
    output_file << ">repeat1\n" <<fasta.data[0].second.substr(start_i, length) << std::endl 
      << ">repeat2\n" << fasta.data[0].second.substr(start_j, length) << std::endl;
  }

  
  output_file.close();
  return 0;
}