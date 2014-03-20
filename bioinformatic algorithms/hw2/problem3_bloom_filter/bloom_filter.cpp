#include "bloom_filter.h"

void BloomFilter::setBytes(std::string const &kmer) {
  for (std::vector<HashFunction>::iterator iter = hash_functions.begin(); iter != hash_functions.end(); ++iter) {
    int h = (*iter).getValue(kmer);
    setBytes(h);
  }
}

void BloomFilter::setBytes(int value) {
  if (value < static_cast<int>(_bits.size()) && value >= 0)
    _bits[value] = true;
}

bool BloomFilter::getBytes(int value) {
  if (value < static_cast<int>(_bits.size()) && value >= 0) {
    return _bits[value];
  }
  return false;
}

void BloomFilter::constructHashFunctions(int kmer_size) {
  //we construct hash functions, _k equals _m/n*log(2)
  hash_functions.clear();
  for (int i = 0; i < _k ; i++) {
    HashFunction hash(_m, kmer_size);
    while (true) {
      bool has_duplicated = false;
      for (std::vector<HashFunction>::iterator iter = hash_functions.begin(); iter != hash_functions.end(); ++iter) {
        if (hash == *iter)
          has_duplicated = true;
      }
      if (!has_duplicated)
        break;
      hash.regenerateCoefficients();
    }
    hash_functions.push_back(hash);
  }
}

int BloomFilter::findPrimeNextTo(int k) {
  bool prime = true;
  int current_candidate = k;
  while (true) {
    prime = current_candidate % 2 != 0;
    for (int i = 3; i <= current_candidate/2; i += 2) {
      if (current_candidate % i == 0) {
        prime = false;
        break;
      }
    }
    if (prime)
      break;
    current_candidate++;
  }
  return current_candidate;
}

//set up some bits in bloom filter
void BloomFilter::Build(StringContainer const & kmers_container) {
  for (StringContainer::iterator iter = kmers_container.begin(); iter != kmers_container.end(); ++iter) {
    setBytes(*iter);
  }
}

double BloomFilter::getProbability() {
  return _probability;
}

bool BloomFilter::isKmerInString(std::string const & tested_sample) {
  bool result = true;
  for (std::vector<HashFunction>::iterator iter = hash_functions.begin(); iter != hash_functions.end(); ++iter) {
    int h = (*iter).getValue(tested_sample);
    result &= getBytes(h);
  }
  return result;
}