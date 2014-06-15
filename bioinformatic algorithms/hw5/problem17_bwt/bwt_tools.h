#pragma once
#include <set>
#include <vector>
#include <string>
#include <memory>
#include "suffix_array.h"



//
class BWTBuilder{
  public:
    BWTBuilder(): alphabet_size(5) { }

    void Build(std::string const& str) {
      text_size = str.size();
      if (str.size() < 1)
        return;
      std::basic_string<size_t> st;
      _suffix_array.resize(text_size, 0);
      st.resize(text_size + 3, 0);
      size_t i = 0;
      _counts.resize(alphabet_size, 0);
      _positions.resize(alphabet_size, 0);
      for (i = 0; i < text_size; i++) {
          st[i] = getCharCode(str[i]);
          _counts[getCharCode(str[i])] ++;
      }

      suffixArray(st, _suffix_array, text_size, 4);
      _fcolumn.resize(text_size);
      _lcolumn.resize(text_size);
      _occurences.resize(alphabet_size, std::vector<size_t>(text_size, 0));

      for (i = 0; i < text_size; i++) {
        _fcolumn[i] = str[_suffix_array[i]];
        _lcolumn[i] =  str[(_suffix_array[i] > 0 ? _suffix_array[i] : text_size ) - 1];
//          std::cout << _suffix_array[i] << std::endl;
        if (i > 0) {
          for (size_t j = 0; j < alphabet_size; j++) {
            _occurences[j][i] = _occurences[j][i - 1];
          }
        }
        _occurences[getCharCode(_lcolumn[i])][i] ++;
      }
      size_t sum = 0;
      for (size_t i = 0; i < alphabet_size; i ++) {
        _positions[i] = sum;
        sum = sum + _counts[i];
      }
//      std::cout << _fcolumn << std::endl  << _lcolumn << std::endl;
    }


    std::set<size_t> find(std::string const& str) {
      if (str.size() < 1 || text_size < 1)
        return std::set<size_t>();
      size_t char_code = getCharCode(str.back());
      size_t left = 0, right = text_size - 1;

      for (size_t i = 0; i < str.size(); i ++) {
        char_code = getCharCode(str[str.size() - 1 - i]);
        if (_counts[char_code] == 0)
          return std::set<size_t>();
        left = _positions[char_code] + (left > 0 ? _occurences[char_code][ left - 1]:0);
        right = _positions[char_code] + _occurences[char_code][right] -1;
        if (left > right || right > text_size)
          return std::set<size_t>();
      }

      std::set<size_t> result;

      for (size_t i = left; i <= right; i++) {
        result.insert(_suffix_array[i]);
      }
      return result;
      //
    }

    int const getCharCode(char a)  {
      if (a == 'A' || a == 'a')
        return 0;
      if (a == 'C' || a == 'c')
        return 1;
      if (a == 'G' || a == 'g')
        return 2;
      if (a == 'T' || a == 't')
        return 3;
      return 4;
    }

    char getByCode(size_t i) {
      std::string s = "ACGT$";
      if (i < 0 || i >= s.size())
        return '$';
      return s[i];
    }
  private:
    size_t alphabet_size;
    size_t text_size;
    std::basic_string<size_t> _suffix_array;//pointer to suffix array
    std::string _fcolumn, _lcolumn;
    std::vector<size_t> _counts, _positions;
    //std::basic_string<size_t> magic_t;
    std::vector<std::vector<size_t> > _occurences;
};
