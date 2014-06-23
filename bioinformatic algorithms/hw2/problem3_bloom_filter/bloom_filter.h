#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include "hash_functions.h"
#include "helper_functions.h"

class BloomFilter {
  size_t _n;
  double _probability;
  size_t _m;
  size_t _k;
  int _kmer_size;
  std::vector<HashFunction> hash_functions;
  std::vector<bool> _bits;//inner bits container

  void setBytes(std::string const& kmer);

  //functions set bytes in bloom filter for given integer value, gets called by setBytes(std::string const &kmer)
  void setBytes(size_t value);

  bool getBytes(size_t value);

  void constructHashFunctions(int kmer_size);

  size_t findPrimeNextTo(size_t k);

public:
  BloomFilter(size_t n, int samples_count, int kmer_size): _n(n), _probability(0.1), _kmer_size(kmer_size) {
    _m = findPrimeNextTo(static_cast<size_t>(-1.0*n*log(_probability)/(log(2.0)*log(2.0))));
    _k = static_cast<size_t>(ceil(1.0*_m/_n*log(2.0)));
    _bits.resize(_m, false);
    constructHashFunctions(kmer_size);
    _probability = pow(1 - exp(- 1.0*_k*_n/_m), (int)_k);
  }

  //set up some bits in bloom filter
  void Build(StringContainer const & kmers_container);

  double getProbability();

  bool isKmerInString(std::string const & tested_sample);

};
