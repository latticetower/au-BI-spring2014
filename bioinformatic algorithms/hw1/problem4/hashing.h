#pragma once
#include <string>
#include <vector>
#include <set>
#include <iostream>

class RevComplHasher {
  public:
    RevComplHasher(size_t amount = 15): HASHES_AMOUNT(amount), HASHES_AMOUNT2(4) {
      powers.clear();
      size_t pow = 4;
      for (size_t i = 0; i < amount; i ++) {
        powers.push_back(pow);
        pow <<= 2;
      }
    }

    //returns true if hash for main str part and compl rev string part are the same
    inline bool is_match(size_t a, size_t rev_a, size_t power) {
      size_t res =  (((a ^ rev_a) % power)^(power - 1));
      return res==0;
    }

    void PrepareHash(std::string const & str) {
      string_size = str.size();
      hash_container.clear();
      hash_container.resize(HASHES_AMOUNT, std::vector<size_t>(string_size, 0));
      reverse_hash_container.resize(HASHES_AMOUNT, std::vector<size_t>(string_size, 0));

      //these are for skipping positions:
      next_positions.resize(HASHES_AMOUNT2);
      std::vector<std::vector<size_t> > last_positions(HASHES_AMOUNT2);

      for (size_t i = 0; i < HASHES_AMOUNT2; i++) {
        last_positions[i].resize(powers[i], -1);
        next_positions[i].resize(string_size, -1);
      }
      
      //prepare hashes:
      for (size_t i = 0; i < string_size; i++) {
        size_t current_letter = get_letter_hash(str[i]);
        size_t rev_letter = get_letter_hash(str[string_size - i - 1]);
        for (size_t k = 0; k < HASHES_AMOUNT; k++) {
          if (i > k) {
            hash_container[k][i - k] = ((hash_container[k][i - k - 1] << 2) % powers[k]) ^ current_letter;
            reverse_hash_container[k][i - k] = ((reverse_hash_container[k][i-k-1] << 2) % powers[k]) ^ rev_letter;
          }
          else {
            hash_container[k][0] = (hash_container[k][0] << 2) ^ current_letter;
            reverse_hash_container[k][0] = (reverse_hash_container[k][0] << 2) ^ rev_letter;
          }
          // set next position - only for one of directions
          if (i >= k && k < HASHES_AMOUNT2) {
            size_t res = hash_container[k][i - k]; //we are not interested in mismatches now
            if (last_positions[k][res] < string_size && last_positions[k][res] < i - k) 
              next_positions[k][last_positions[k][res]] = i - k;
            last_positions[k][res] = i - k;
          }
          //
        }
      }
    }

    //method returns pair - start position and length
    std::pair<size_t, size_t> get_max_substr(size_t min_length, size_t max_length, size_t last_pos) {
      if (max_length > min_length + 1) {
        size_t median_length = min_length + (max_length - min_length) / 2;
        size_t pos = get_substr(median_length);
        return (pos < string_size? get_max_substr(median_length, max_length, pos) : get_max_substr(min_length, median_length, -1));
      }
      return std::make_pair(last_pos, min_length);
    }
    

    //method returns index of substring of given length
    size_t get_substr(size_t median_length) {
      std::cout << "get substr called with length "<< median_length << std::endl;
      if (median_length == 0)
        return 0;
      //prepare rolling hash, modulo great prime (> 2^15 = first found via wolframalpha - 1073742037)
      
      std::vector<size_t> hash, rev_hash;
      size_t buf = 0, rev_buf = 0;
      //suppose median length is greater than HASHES_AMOUNT.
     // size_t k = std::min(HASHES_AMOUNT, median_length);
      size_t i = 0;
      for (i = 0; i < median_length; i ++) {
        buf = (buf + hash_container[0][i]) % GREAT_PRIME;
        rev_buf = (rev_buf + ((powers[0]-1) ^ reverse_hash_container[0][i])) % GREAT_PRIME;
      }
      hash.push_back(buf);
      rev_hash.push_back(rev_buf);
      for (; i < string_size; i++) {
        buf = (buf + hash_container[0][i] - hash_container[0][i - median_length]) % GREAT_PRIME;
        hash.push_back(buf);
        rev_buf = (rev_buf + ((powers[0]-1) ^ reverse_hash_container[0][i]) - ((powers[0]-1) ^ reverse_hash_container[0][i - median_length])) % GREAT_PRIME;
        rev_hash.push_back(rev_buf);
      }

      //iterate for each position in reverse string
      for (size_t rev_i = 0; rev_i < string_size; rev_i ++) {
        //rev_i is the beginning of current hash in reverse string.
        // in normal hash it is equal to position string_size - rev_i - 1.
        // and last element is string_size-rev_i-median_length
        for (size_t i =  get_next(rev_i, 0, 0, median_length, true); i < string_size; i =  get_next(rev_i, 0, i, median_length, false)) {
          if (CheckLengthRevCompl(i, rev_i, median_length, hash, rev_hash))
            return i;
        }
      }
      return -1;
    }


    //helper method. returns next position of hash prefix equal to last one
    size_t get_next(size_t rev_i, size_t min_pos, size_t last_found, size_t length, bool first_call) {
      //std::cout << i << " " << min_pos << " " << last_found << " " << length << std::endl;
      /*if (i > min_pos) {
        std::cout << "got wrong min_pos" << std::endl;
        return -1;
      }
      if (last_found < min_pos && last_found > i) {
        std::cout << "got wrong last_found" << std::endl;
        return -1;
      }*/
      //std::cout << rev_i << " " << min_pos << " " << last_found << " " << length << " " << first_call << std::endl;
      size_t k = std::min(length, HASHES_AMOUNT2);
      
      if (k == 0) 
        k = 1;
      
      if (first_call || last_found < min_pos) {
        size_t k = std::min(length, HASHES_AMOUNT);
        if (hash_container.size() < k || hash_container[k-1].size() <= rev_i) {
          std::cout << "got some problem with hash_container1"<<std::endl;
        }
        size_t rev_val = reverse_hash_container[k - 1][rev_i];
        size_t current = min_pos;
        if (hash_container.size() < k || hash_container[k - 1].size() <= current) {
          std::cout << "got some problem with hash_container2"<<std::endl;
        }
        while (current < string_size && !is_match(rev_val, hash_container[k - 1][current], powers[k - 1])) {
          if (current + rev_i + 1 >= string_size)
            return string_size + 1;
          current++;
          if (hash_container.size() < k || hash_container[k-1].size() <= current) {
             std::cout << "got some problem with hash_container3"<<std::endl;
          }

        }
        return current;
      }
      if (last_found < string_size) {
        if (hash_container.size() < k || hash_container[k - 1].size() <= last_found) {
          std::cout << "got some problem with hash_container last_found" << std::endl;
        }
        return next_positions[k - 1][last_found];
      }
      return last_found;
    } 

  public:
  


    bool CheckLengthRevCompl(size_t i, size_t rev_i, size_t median_length, std::vector<size_t> const & hash, std::vector<size_t> const & rev_hash, bool is_full_length = true) {
      //0. check if rev_i > string_length
      if (rev_i > string_size || i > string_size) 
        return false;
      if (median_length == 0)
        return true;

      //1. check if they overlap and return false
      if (i + median_length + rev_i + median_length > string_size) {
        return false;
      }
      if (is_full_length && hash[i] != rev_hash[rev_i])
        return false;
      //check for collisions:
      size_t k = std::min(HASHES_AMOUNT, median_length);
      if (is_match(hash_container[k - 1][i], this->reverse_hash_container[k - 1][rev_i], powers[k - 1]))
        return CheckLengthRevCompl(i + k, rev_i + k, median_length - k, hash, rev_hash, false); 
      // in recursive call don't check rolling hash values, use precise hashes instead
      return false;
    }

    


  protected:
    bool chars_equal(size_t i, size_t j) const {
      if (i >= hash_container[0].size() || j >= hash_container[0].size())
        return false;
      return hash_container[0][i] == hash_container[0][j];
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
    std::vector<std::vector<size_t> > reverse_hash_container;
    std::vector<std::vector<size_t> > next_positions;
    static const size_t GREAT_PRIME = 1073742037;//great prime (> 2^15 = found via wolframalpha - 1073742037)
};