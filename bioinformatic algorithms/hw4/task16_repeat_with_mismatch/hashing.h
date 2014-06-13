#pragma once
#include <string>
#include <vector>

class Hasher {
  public:
    Hasher(size_t amount = 15): HASHES_AMOUNT(amount) {
      powers.clear();
      size_t pow = 4;
      for (size_t i = 0; i < amount; i ++) {
        powers.push_back(pow);
        pow <<= 2;
      }
    }

    void PrepareHash(std::string const & str) {
      string_size = str.size();
      hash_container.clear();
      hash_container.resize(HASHES_AMOUNT, std::vector<size_t>(string_size, 0));
      next_positions.resize(string_size, -1);
      std::vector<size_t> last_positions(4, -1);

      for (size_t i = 0; i < string_size; i++) {
        size_t current_letter = get_letter_hash(str[i]);
        for (size_t k = 0; k < HASHES_AMOUNT; k++) {
          if (i > k) {
            hash_container[k][i - k] = ((hash_container[k][i - k - 1] << 2) % powers[k]) ^ current_letter;
          }
          else {
            hash_container[k][0] = (hash_container[k][0] << 2) ^ current_letter;
          }
        }
        // set next position
        if (last_positions[current_letter] < string_size)
          next_positions[last_positions[current_letter]] = i;
        last_positions[current_letter] = i;
      }
    }
    size_t get_next(size_t i, size_t min_pos, size_t last_found) {
      if (last_found < string_size) {
        return next_positions[last_found];
      }
      if (last_found == 0) {
        size_t current = next_positions[i];
        while (current < min_pos) {
          current = next_positions[current];
        }
        return current;
      }
      return last_found;
    } 
  private:
    std::vector<size_t> next_positions;
  public:
    bool CheckEquality(size_t i, size_t j, size_t length, bool allow_mismatch = true) {
      CheckResult ch = CheckEquality1(i, j, length, allow_mismatch);
      return ((allow_mismatch && ch == CheckResult::MISMATCH) || ch == CheckResult::EQUAL);
    }

    enum CheckResult {EQUAL, MISMATCH, NOTEQUAL, EOSREACHED, OVERLAP};
    
    //methods checks if substrings of given length are equal, and return number of mismatches
    // returns 0 if they equal
    // returns 1 if founds 1 mismatch
    // returns 2 if there are more than 1 mismatch (or there is no way to compare them)
    CheckResult CheckEquality1(size_t i, size_t j, size_t length, bool allow_mismatch = true) {
      if (i > j)
        return CheckEquality1(j, i, length);
      if (j + length >= string_size)
        return CheckResult::EOSREACHED;
      if (i + length > j) // this means strings overlap
        return CheckResult::OVERLAP;
      if (length < HASHES_AMOUNT) {
        if (hash_container[length - 1][i] == hash_container[length - 1][j]) {
          return EQUAL;
        }
        else {
          if (allow_mismatch)
            return is_single_mismatch(i, j, length);
          return CheckResult::NOTEQUAL;
        }
      }
      else {
        if (hash_container[HASHES_AMOUNT - 1][i] == hash_container[HASHES_AMOUNT - 1][j])
          return CheckEquality1(i + HASHES_AMOUNT, j + HASHES_AMOUNT, length - HASHES_AMOUNT);
        else {
          if (allow_mismatch && is_single_mismatch(i, j, HASHES_AMOUNT - 1) == CheckResult::MISMATCH) {
            return CheckEquality1(i, j, length - HASHES_AMOUNT, false);
          }
          return CheckResult::NOTEQUAL;
        }
      }
    }

    //method returns:
    //zero if strings are not equal
    // expected_length if they are equal
    // max length if they can be extended
    size_t CheckLength(size_t i, size_t j, size_t expected_length, bool allow_mismatch = true) {
      if (i > j)
        return CheckLength(j, i, expected_length, allow_mismatch);
      if (j + expected_length >= string_size)
        return 0;// string could not be extended, we don't need it. we already know some string of expected_length
      if (i + expected_length > j) {
        CheckResult ch = CheckEquality1(i, j, expected_length, allow_mismatch);
        if (ch == CheckResult::EQUAL || ch == CheckResult::MISMATCH)
          return expected_length;
        return 0;
      }
      if (chars_equal(i + expected_length, j + expected_length)) {
        return FindLength(i, j, j, true);
      }
      else {
        if (j + expected_length + 1 >= string_size)
          return 0;
        if (i + expected_length + 1 == j)
          return FindLength(i, j, j, allow_mismatch);
        if (allow_mismatch && chars_equal(i + expected_length + 1, j + expected_length + 1)) {
          return FindLength(i, j, j, allow_mismatch);
        }
        return 0;
      }
      return 0;

    }

    //method finds expected string length and returns it
    // j_start is the start position of the second string
    size_t FindLength(size_t i, size_t j_start, size_t j, bool allow_mismatch = true) {
      size_t length = 0;
      if (i == j_start)
        return 0;
      if (j_start - i <= HASHES_AMOUNT) {//checks overlap
        return find_length(i, j, j_start - i, allow_mismatch);
      }
      
      if (hash_container[HASHES_AMOUNT - 1][i] == hash_container[HASHES_AMOUNT - 1][j]) {
        return HASHES_AMOUNT + FindLength(i + HASHES_AMOUNT, j_start, j + HASHES_AMOUNT, allow_mismatch);
      }
      else {
        size_t res = find_length(i, j, HASHES_AMOUNT, allow_mismatch);
        return (res < HASHES_AMOUNT ? res : FindLength(i + HASHES_AMOUNT, j_start, j + HASHES_AMOUNT, false));
      }
    }


  protected:
    bool chars_equal(size_t i, size_t j) const {
      if (i >= hash_container[0].size() || j >= hash_container[0].size())
        return false;
      return hash_container[0][i] == hash_container[0][j];
    }

    // for given small string returns if there is one mismatch or not
    CheckResult is_single_mismatch(size_t i, size_t j, size_t length) {
      int error_count = 0;
      for (size_t k = 0; k < length; k++) {
        if (hash_container[0][i + k] != hash_container[0][j + k]) {
          error_count ++;
          if (error_count > 1)
            return CheckResult::NOTEQUAL;
        }
      }
      return (error_count == 1 ? CheckResult::MISMATCH : CheckResult::EQUAL);
    }

    // for given small string returns if there is one mismatch or not
    size_t find_length(size_t i, size_t j, size_t length, bool allow_mismatch) {
      int error_count = 0;
      for (size_t k = 0; k < length; k++) {
        if (hash_container[0][i + k] != hash_container[0][j + k]) {
          error_count ++;
          if (error_count > 1 || !allow_mismatch)
            return k;
        }
      }
      return length;
    }

    size_t get_letter_hash(char a) {
      if (a == 'A' || a == 'a')
        return 0;
      if (a=='C' || a == 'c')
        return 1;
      if (a == 'G' || a == 'g')
        return 2;
      if (a == 'T' || a == 't')
        return 3;
      return 4;//or should return error
    }
  private:
    size_t HASHES_AMOUNT;
    size_t string_size;
    std::vector<size_t> powers;
    std::vector<std::vector<size_t> > hash_container;
};