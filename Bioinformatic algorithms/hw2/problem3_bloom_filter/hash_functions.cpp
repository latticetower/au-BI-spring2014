#include<time.h>
#include<cstdlib>
#include "hash_functions.h"

void HashFunction::generateCoefficients() {
  coefficients.clear();
  srand(time(NULL));
  for (int i = 0; i < _keys_count; i++) {
    coefficients.push_back(rand() % _modulo);
  }
}

void HashFunction::regenerateCoefficients() {
  srand(time(NULL));
  for (int i = 0; i < _keys_count; i++) {
    coefficients[i] = rand() % _modulo;
  }
}

int HashFunction::getValue(std::string  const & kmer) {
  int result = 0;
  for (int i = 0; i < static_cast<int>(kmer.size()); i++) {
    result += coefficients[i]*kmer[i];
    result %= _modulo;
  }
  return result;
}

bool HashFunction::operator ==(HashFunction const& hash) {
  if (this->_keys_count != hash._keys_count || this->_modulo != hash._modulo)
    return false;

  bool result = true;
  for (int i = 0; i < _keys_count; i++) {
    result &= (this->coefficients[i] == hash.coefficients[i]);
  }
  return result;
}