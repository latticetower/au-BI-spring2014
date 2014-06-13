#pragma once
#include <string>
#include <vector>
#include <set>

class Hasher {
  public:
    Hasher(size_t amount = 15): HASHES_AMOUNT(amount), HASHES_AMOUNT2(4) {
      powers.clear();
      size_t pow = 4;
      for (size_t i = 0; i < amount; i ++) {
        powers.push_back(pow);
        pow <<= 2;
      }
    }

    inline bool is_mismatch(size_t a, size_t b) {
      size_t res = a^b;
      if (res == 0)
        return true;
      size_t res2 = 0;
      while (res > 4) {
        res2 = res % 4;
        res >>= 2;
      }
      return !(res2 && res);
    }

    void PrepareHash(std::string const & str) {
      string_size = str.size();
      hash_container.clear();
      hash_container.resize(HASHES_AMOUNT, std::vector<size_t>(string_size, 0));
      next_positions.resize(HASHES_AMOUNT2);

      std::vector<std::vector<size_t> > last_positions(HASHES_AMOUNT2);
      for (size_t i = 0; i < HASHES_AMOUNT2; i++) {
        last_positions[i].resize(powers[i], -1);
        next_positions[i].resize(string_size, -1);
      }

      for (size_t i = 0; i < string_size; i++) {
        size_t current_letter = get_letter_hash(str[i]);
        for (size_t k = 0; k < HASHES_AMOUNT; k++) {
          if (i > k) {
            hash_container[k][i - k] = ((hash_container[k][i - k - 1] << 2) % powers[k]) ^ current_letter;
          }
          else {
            hash_container[k][0] = (hash_container[k][0] << 2) ^ current_letter;
          }
          // set next position
          if (i >= k && k < HASHES_AMOUNT2) {
            std::set<size_t> res = get_all_with_one_mismatch(hash_container[k][i - k], powers[k], k + 1); 
            for (std::set<size_t>::iterator iter = res.begin(); iter != res.end(); ++iter) {
              if (last_positions[k][*iter] < string_size && last_positions[k][*iter] < i - k) 
                next_positions[k][last_positions[k][*iter]] = i - k;
              last_positions[k][*iter] = i - k;
              //last_positions[k][*iter] = i - k;
            }
            
          }
          //
        }
       
      }
    }

    // value is the value for witch we construct hash.
    // length is the current hash 
    std::set<size_t> get_all_with_one_mismatch(size_t value, size_t power, size_t length) {
      std::set<size_t> result;
      //if (length <= 1) {
      //  result.insert(value % power);
      //  return result;
      //}
      for (size_t i = 0; i < length; i ++) {
        for (size_t k = 0; k < 4; k++) {
          result.insert((value & ((power - 1) ^ (3 << 2*i))) ^ (k << 2*i));
        }
      }
      return result;
    }

    size_t get_next(size_t i, size_t min_pos, size_t last_found, size_t length) {
      //std::cout << i << " " << min_pos << " " << last_found << " " << length << std::endl;
      if (i > min_pos) {
        std::cout << "got wrong min_pos" << std::endl;
        return -1;
      }
      if (last_found < min_pos && last_found > i) {
        std::cout << "got wrong last_found" << std::endl;
        return -1;
      }
      size_t k = std::min(length, HASHES_AMOUNT2);
      
      if (k == 0) 
        k = 1;
      
      if (last_found < min_pos) {
        size_t k = std::min(length, HASHES_AMOUNT);
        if (hash_container.size() < k || hash_container[k-1].size() <= i) {
          std::cout << "got some problem with hash_container1"<<std::endl;
        }
        size_t val0 = hash_container[k - 1][i];
        size_t current = min_pos;
        if (hash_container.size() < k || hash_container[k-1].size() <= current) {
          std::cout << "got some problem with hash_container2"<<std::endl;
        }
        while (current< string_size && !is_mismatch(val0, hash_container[k - 1][current])) {
          current++;
          if (hash_container.size() < k || hash_container[k-1].size() <= current) {
             std::cout << "got some problem with hash_container3"<<std::endl;
           }
        }
        return current;
      }
      if (last_found < string_size) {
        if (hash_container.size() < k || hash_container[k-1].size() <= last_found) {
          std::cout << "got some problem with hash_container last_found"<<std::endl;
        }
        return next_positions[k - 1][last_found];
      }
      return last_found;
    } 
  private:
    std::vector<std::vector<size_t> > next_positions;
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
      if (j + expected_length > string_size)
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
          return FindLength(i, j, j, allow_mismatch);
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
      if (i == j_start)
        return 0;
      if (j_start - i <= HASHES_AMOUNT || string_size - j <= HASHES_AMOUNT) {//checks overlap
        return find_length(i, j, std::min(string_size - j, j_start - i), allow_mismatch);
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
      if (i > j)
        return find_length(j, i, length, allow_mismatch);
      if (j + length > string_size)
        return find_length(i, j, (string_size > j ? string_size - j : 0), allow_mismatch);
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
    size_t HASHES_AMOUNT2;
    size_t string_size;
    std::vector<size_t> powers;
    std::vector<std::vector<size_t> > hash_container;
};