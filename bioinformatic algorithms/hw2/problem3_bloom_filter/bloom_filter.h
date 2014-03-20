#pragma once
#include <vector>
#include <cmath>
#include "hash_functions.h"
#include "helper_functions.h"

class BloomFilter {
  int _n;
  double _probability;
  int _m;
  int _k;
  int _kmer_size;
  std::vector<HashFunction> hash_functions;
  std::vector<bool> _bits;//inner bits container

  void setBytes(std::string const &kmer);

  //functions set bytes in bloom filter for given integer value, gets called by setBytes(std::string const &kmer)
  void setBytes(int value);

  bool getBytes(int value);

  void constructHashFunctions(int kmer_size);

  int findPrimeNextTo(int k);

public:
  BloomFilter(int n, int samples_count, int kmer_size): _n(n), _kmer_size(kmer_size) {
    _m = findPrimeNextTo(n + samples_count + 1);
    _k = static_cast<int>(ceil(1.0*_m/_n*log(2.0)));
    _bits.resize(_m, false);
    constructHashFunctions(kmer_size);
    _probability = pow(1 - exp(- 1.0*_k*_n/_m), _k);
  }

  //set up some bits in bloom filter
  void Build(StringContainer const & kmers_container);

  double getProbability();

  bool isKmerInString(std::string const & tested_sample);

};