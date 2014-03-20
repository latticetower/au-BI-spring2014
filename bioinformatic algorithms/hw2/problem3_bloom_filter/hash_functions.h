#pragma once
#include<vector>
#include<string>

// class incapsulates hash function logic
class HashFunction{
  int _modulo;
  int _keys_count;
  std::vector<int> coefficients;

  void generateCoefficients();
public:
  HashFunction(int modulo, int keys_count): _modulo(modulo), _keys_count(keys_count) {
    generateCoefficients();
  }
  void regenerateCoefficients();
  int getValue(std::string  const & kmer);
  bool operator == (HashFunction const& hash);
};
