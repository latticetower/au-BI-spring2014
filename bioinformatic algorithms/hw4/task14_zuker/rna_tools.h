#pragma once
#include <string>
#include <iostream>
#include <vector>

// this file contains algorithms for RNA secondary structure prediction
// Nussinov algorithm implementation
typedef double ValueType;
typedef std::pair<size_t,std::string> ResultElType;

struct data_element{
  data_element(): cost(0), is_bp(false) {
    positions.clear();
  }
  void set_pos(size_t i, size_t j) {
    positions.clear();
    positions.push_back(std::make_pair(i, j));
  }
  void add_pos(size_t i, size_t j) {
    positions.push_back(std::make_pair(i, j));
  }

  void clear_pos() {
    positions.clear();
  }
  ValueType cost;
  std::vector<std::pair<size_t, size_t> >positions;
  bool is_bp;
};

class RNAPredictor {
    size_t min_loop_size;
    ValueType stacked_score_cost;
  protected:
    bool is_base_pair(char a, char b) {
      if (a > b)
        return is_base_pair(b, a);
      return ((a == 'A' && b == 'U') || (a == 'C' && b == 'G'));
    }


    ValueType get_cost_for_base_pair(std::string const& str,size_t i, size_t j) {
      ValueType cost = structure_matrix[i + 1][j - 1].cost;
      if (is_base_pair(str[i], str[j])) {
        if (j - i <= min_loop_size)
          return 0;
        cost = structure_matrix[i + 1][j - 1].cost + 1;
        if (is_base_pair(str[i + 1], str[j - 1]) && (j - 1) - (i + 1) > min_loop_size)
          cost += stacked_score_cost;
        return cost;
      }
      return cost;
    }
    
    ValueType get_cost_for_bifurcation(size_t i, size_t j, size_t & k_) {
      //should return max value for all k
      k_ = 0;
      ValueType max_value = 0;
      for (size_t k = i + 1; k < j; k++) {
        ValueType new_max = structure_matrix[i][k].cost + structure_matrix[k + 1][j].cost;
        if (new_max > max_value) {
          max_value = new_max;
          k_ = k;
        }
      }
      return max_value;
    }
    
    void init_data(size_t data_size) {
      if (data_size == 0)
        return;
      structure_matrix.resize(data_size);
      for (size_t i = 0; i < data_size; i++) {
        structure_matrix[i].resize(data_size); //TODO: optimize data usage - almost half of the matrix is not needed!
      }
    }
    
    void process_all(std::string const& str) {
      if (str.size() == 0)
        return;
      for (size_t pos = 1; pos < str.length(); pos++) {
        for (size_t i = 0; i + pos < str.length(); i++) {
          structure_matrix[i][i + pos].cost = std::max(structure_matrix[i][i + pos - 1].cost,
                                                       structure_matrix[i + 1][i + pos].cost
          );
          if (structure_matrix[i][i + pos].cost == structure_matrix[i][i + pos - 1].cost) {
            structure_matrix[i][i + pos].set_pos(i, i + pos - 1);
          }
          else {
            structure_matrix[i][i + pos].set_pos(i + 1, i + pos);
          }
          ValueType max_value = get_cost_for_base_pair(str, i, i + pos);
          if (structure_matrix[i][i + pos].cost <= max_value && max_value > 0) {
            structure_matrix[i][i + pos].cost = max_value;
            structure_matrix[i][i + pos].set_pos(i + 1, i + pos - 1);
            //i + 1], str[j - 1
            structure_matrix[i][i + pos].is_bp = is_base_pair(str[i], str[i + pos]);
          }
          size_t k;
          max_value = get_cost_for_bifurcation(i, i + pos, k);
          if (structure_matrix[i][i + pos].cost < max_value && max_value > 0) {
            structure_matrix[i][i + pos].cost = max_value;
            structure_matrix[i][i + pos].set_pos(i, k);
            structure_matrix[i][i + pos].add_pos(k + 1, i + pos);
           // structure_matrix[i][i + pos].is_bp = false;
          }
          //next conditions are for backtracing
        }
      }
      //
    }
  public:
    RNAPredictor(size_t _loop_size, ValueType _score_cost): min_loop_size(_loop_size), stacked_score_cost(_score_cost) {
      
    }
    
    void processSeq(std::string const& rna_sequence) {
      if (rna_sequence.size() == 0)
        return;
      init_data(rna_sequence.length());
      process_all(rna_sequence);
    }

    void debug_print(std::ostream& os) {
      os << "structure matrix is "<<std::endl;
      for (size_t i = 0; i< structure_matrix.size(); i++) {
        for (size_t j = 0; j < structure_matrix[i].size(); j++) {
          os << structure_matrix[i][j].cost << " ";
        }
        os << std::endl;
      }
    }

    std::vector<std::pair<ResultElType, ResultElType> > get_result(size_t current_i, size_t current_j, std::string const& str) {
      std::vector<std::pair<ResultElType, ResultElType> > result;
      if (str.size() == 0)
        return result;
      
      size_t i = current_i, 
             j = current_j,
             length = 0,
             start1 = -1, 
             start2 = -1;

      while (j - i >= this->min_loop_size && structure_matrix[i][j].positions.size() > 0){
        std::vector<std::pair<size_t, size_t> > next = structure_matrix[i][j].positions;
        if ((!structure_matrix[i][j].is_bp || next.size() > 1) && length > 0) {
            // save current result to result array and clear start positions and length
            result.push_back(std::make_pair(std::make_pair(start1, str.substr(start1, length)),
                                            std::make_pair(start2, str.substr(start2, length))
              ));
            //then clear temp variables
            length = 0;
            start1 = -1;
            start2 = -1;
        }
        
        if (structure_matrix[i][j].is_bp) {//append base_pair to i
          start1 = std::min(start1, i);
          start2 = std::min(start2, j);
          length ++;
        }

        if (next.size() > 1) {
          for (std::vector<std::pair<size_t, size_t> >::iterator iter = next.begin(); iter!= next.end(); ++iter) {
            std::vector<std::pair<ResultElType, ResultElType> > r = get_result(iter->first, iter->second, str);
            result.insert(result.begin(), r.begin(), r.end());
          }
          break;
        }
        if (next.size () == 1) {
          i = next[0].first;
          j = next[0].second;
        }
        if (next.size() == 0) {//just in case of unrealistic results
          std::cout << "got some error while processing input sequence\n";
          break;
        }
      }
      //after main loop, something may be unprocessed
      if (length > 0) {//add base pair info
        result.push_back(std::make_pair(std::make_pair(start1, str.substr(start1, length)),
                                        std::make_pair(start2, str.substr(start2, length))
          ));
        // no need to clear temp variables
      }
      return result;
    }

  private:
     std::vector<std::vector<data_element> > structure_matrix;//contains matrix for nussinov algo
};