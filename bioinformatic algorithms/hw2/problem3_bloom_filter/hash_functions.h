#pragma once
#include<vector>
#include<string>

// class incapsulates hash function logic
class HashFunction{
  size_t _modulo;
  size_t _keys_count;
  std::vector<size_t> coefficients;

  void generateCoefficients();
public:
  HashFunction(size_t modulo, size_t keys_count): _modulo(modulo), _keys_count(keys_count) {
    generateCoefficients();
  }
  void regenerateCoefficients();
  size_t getValue(std::string  const & kmer);
  bool operator == (HashFunction const& hash);
};
