#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include "fasta_data.h"
#include "helper_functions.h"
#include "bloom_filter.h"

StringContainer get_kmers(std::string &sample_string, int kmer_size) {
  StringContainer kmers_container;
  
  for (int i = 0; i < static_cast<int>(sample_string.size()) - kmer_size + 1; i ++) {
    kmers_container.insert(sample_string.substr(i, kmer_size));
  }
  return kmers_container;
}

bool read_second_file(char const *input_filename, int& kmer_size, std::vector<std::string> &test_samples) {
  std::ifstream input_file2(input_filename);
  if (input_file2.fail()) {
    return false;
  }
  input_file2 >> kmer_size;
  std::string buffer = "";

  while (!input_file2.eof()) {
    std::getline(input_file2, buffer);
    buffer.erase(std::remove_if(buffer.begin(), buffer.end(), ::iscntrl), buffer.end());//removes carriage return from buffer - just in case
    std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower);
    if (buffer.size() > 0)
      test_samples.push_back(buffer);
  }
  input_file2.close();
  return true;
}


int main(int argc, char**argv) {
  FastaData fasta;
  if (argc < 4) {
    std::cout << "Please, provide input files name as a first 2 parameters, and output file name as a third parameter" << std::endl;
    return 0;
  }
  if (!fasta.loadFromFile(argv[1])) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  //read data from second file:
  int kmer_size;
  std::vector<std::string > test_samples;

  if (!read_second_file(argv[2], kmer_size, test_samples) ){
    std::cerr << "got error while opening second data file";
    return 1;
  }
  StringContainer kmers_container = get_kmers(fasta.data.front().second, kmer_size);
  //
  BloomFilter bloom_filter(kmers_container.size(), test_samples.size(), kmer_size);
  bloom_filter.Build(kmers_container);
  //output
  std::ofstream output_file(argv[3]);
  output_file.precision(2);
  output_file.setf(std::ios::showpoint);
  output_file.setf(std::ios::fixed);
  if (kmers_container.size() < 1) {
    output_file << "input string length is less than kmer size, no kmers were found";
    output_file.close();
    return 0;
  }
  std::vector<int> results;
  int fp_count = 0;
  for (std::vector<std::string>::iterator iter = test_samples.begin(); iter != test_samples.end(); ++iter) {
    if (bloom_filter.isKmerInString(*iter)) {
      results.push_back(1);
      if (kmers_container.find(*iter) == kmers_container.end())
        fp_count ++;
    }
    else {
      results.push_back(0);
    }
  }
  output_file << "FP = " << 100.0*fp_count/results.size() << " % " << std::endl;
  for (std::vector<int>::iterator iter = results.begin(); iter != results.end(); ++iter) {
    output_file << *iter << std::endl;
  }
  output_file.close();
}