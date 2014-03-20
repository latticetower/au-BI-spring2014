#include <iostream>
#include <vector>
#include<string>
#include<algorithm>
#include "fasta_data.h"

void computePalindromes(std::string const& input_string, std::vector<int>& even_palindromes, std::vector<int> & odd_palindromes) {
  //1. compute all odd palindromes in position i for all positions in input string
  int leftBorder = 0, rightBorder = -1, leftBorder2 = 0, rightBorder2 = -1;
  for (int i = 0; i < (int)input_string.size(); i++) {
    int width = 0;
    if (rightBorder > i) {
      width = std::min(1 + static_cast<int>(rightBorder - i), odd_palindromes[leftBorder + (rightBorder - i)]);
    }
    while (i + width < static_cast<int>(input_string.size()) && i - width >= 0 && input_string[i - width] == input_string[i + width]) {
      width ++;
    }
    width --;
    odd_palindromes[i] = width + 1;
    if (i + width > rightBorder) {
      rightBorder = i + width;
      leftBorder = i - width;
    }
    //process even palindromes in current position
    width = 0;
    if (rightBorder2 > i) {
      width += std::min(static_cast<int>(rightBorder2 - i + 1), even_palindromes[leftBorder2 + (rightBorder2 - i + 1)]);
    }
    while (i + width - 1 < static_cast<int>(input_string.size()) && i - width >= 0 && input_string[i - width] == input_string[i + width - 1]) {
      width ++;
    }
    if (width > 0) width --; 
    even_palindromes[i] = width;
    if (i + width - 1 > rightBorder2) {
      rightBorder2 = i + width - 1;
      leftBorder2 = i - width;
    }
  }
}

int processPalindromes(std::vector<int>& even_palindromes, std::vector<int>& odd_palindromes, int l, int r) {
  int result = 0;
  
  for (int i = l; i <= r; i ++) {
    //calculate odd palindromes within range
    result += std::min(odd_palindromes[i], std::min(i - l + 1, r - i + 1));
    //and the same for even palindromes:
    result += std::min(even_palindromes[i], std::min(i - l, r - i + 1));
  }
  return result;
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
  std::string input_string = fasta.data.front().second;
  //compute vector of values - dynamically
  std::vector<int> even_palindromes(input_string.size(), 0), odd_palindromes(input_string.size(), 1);
  computePalindromes(input_string, even_palindromes, odd_palindromes);

  std::ifstream requests_file(argv[2]);
  std::ofstream results_file(argv[3]);
  int n;
  requests_file >> n;
  for (int i = 0; i < n; i++) { 
    int l, r;
    requests_file >> l >> r;
    results_file << processPalindromes(even_palindromes, odd_palindromes, l, r) << std::endl;

  }
  results_file.close();
  return 0;
}