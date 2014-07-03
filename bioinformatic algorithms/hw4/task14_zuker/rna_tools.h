#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include "energies_info.h"

typedef double ValueType;
typedef std::pair<size_t,std::string> ResultElType;

struct data_element{
  data_element(): opt_structure(EnergiesInfo::Structure::None), i1j1_set(false), k1_set(false) { }

  bool is_i1j1_set() { return i1j1_set; }
  bool is_k1_set() { return k1_set; }
	
  //saves positions of bulge loop and changes status
  void set_bulge_loop(size_t i, size_t j) {
    set_bulge_loop();
    i1 = i;
    j1 = j;
    i1j1_set = true;
  }
  //just changes status
  void set_bulge_loop() {
    opt_structure = EnergiesInfo::Structure::BulgeOrLoop;
  }

  //this is set when structure is obtain from shifting W[i][j] from W[i+1][j] or W[i][j-1]
  void set_shift() {
    opt_structure = EnergiesInfo::Structure::Shift;
  }

  // this is set when structure is obtain via combining some W subparts
  void set_w(size_t k) {
    opt_structure = EnergiesInfo::Structure::Composed;
    k1 = k;
    k1_set = true;
  }

 //this is set when struct is recieved from combination of VM's
  void set_vm(size_t k) {
    set_vm();
    k1 = k;
    k1_set = true;
  }
  //the same, doesn't rewrite k1 value
  void set_vm() {
    opt_structure = EnergiesInfo::Structure::ComposedVM;
  }
  //sets hairpin (as stated)
  void set_hairpin() {
    opt_structure = EnergiesInfo::Structure::Hairpin;
  }

  void set_stacked_status() {
    opt_structure = EnergiesInfo::Structure::Stacked;
  }

  void clear() {
    opt_structure = EnergiesInfo::Structure::None;
  }

  EnergiesInfo::Structure opt_structure;
  size_t i1, j1, k1;
  bool i1j1_set, k1_set;
};


class RNAEnergiesPredictor {
  public:
    typedef std::pair<ResultElType, ResultElType> StackedSeqPair;
    typedef std::pair<size_t, size_t> PairedCoords;


    RNAEnergiesPredictor(EnergiesInfo const& ei): energies_info(ei), _a(1) { }

    void ProcessSeq(std::string const& str) {
      processed_str = str;
      string_size = str.size();
      prepare_data();
      for (size_t pos = 1; pos < str.length(); pos++) {
        for (size_t i = 0; i + pos < str.length(); i++) {
          calculate_at(str, i, i + pos);
        }
      }
    }


    double getEnergy()const {
      return W[0][string_size - 1];
    }


    void debug_print_matrix(std::ostream & os, std::vector<std::vector<double> > & matrix) {
      for (size_t i = 0; i < matrix.size(); i ++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
          os << matrix[i][j] << ";";
        }
        os << std::endl;
      }
    }

    void debug_print_matrices(std::ostream & os) {
      os << "W matrix;\n";
      debug_print_matrix(os, W);
      os << "V matrix;\n";
      debug_print_matrix(os, V);
      os << "VBI matrix;\n";
      debug_print_matrix(os, VBI);
      os << "VM matrix;\n";
      debug_print_matrix(os, VM);
    }

  protected:
    
    void prepare_data() {
      W.resize(string_size, std::vector<double>(string_size, EnergiesInfo::INF));
      V.resize(string_size, std::vector<double>(string_size, EnergiesInfo::INF));
      VBI.resize(string_size, std::vector<double>(string_size, 0));
      VM.resize(string_size, std::vector<double>(string_size, 0));
      path.resize(string_size, std::vector<data_element>(string_size));
    }

    //helper method: calculates VM values in (i,j) cell
    void calculate_VM_at(std::string const& str, size_t i, size_t j) {
      //calculate VM
      VM[i][j] = EnergiesInfo::INF;
      for (size_t k = i + 1; k < j - 1; k++) {
        if (VM[i][j] > W[i + 1][k] + W[k + 1][j - 1]) {
          path[i][j].set_vm(k);
          VM[i][j] = W[i + 1][k] + W[k + 1][j - 1];
        }

      }
      //if (VM[i][j] < EnergiesInfo::INF)
      VM[i][j] += _a;
    }

    double getBulgeOrLoop(size_t i, size_t j, size_t i1, size_t j1) {
      if (!(is_compl(i1, j1) && is_compl(i, j)))
          return EnergiesInfo::INF;
      if (j - j1 == 1 || i1 - i == 1) {
        return energies_info.get(EnergiesInfo::Structure::Bulge, std::max(j - j1 - 1, i1 - i - 1));
      }
      else {
        return energies_info.get(EnergiesInfo::Structure::InternalLoop, j - j1 - 1 + i1 - i - 1);
      }
    }

    void calculate_VBI_at(std::string const& str, size_t i, size_t j) {
      VBI[i][j] = EnergiesInfo::INF;
      for (size_t i1 = i + 1; i1 < j - 1; i1++) {
        for (size_t j1 = i1 + 1; j1 < j; j1++) {
          if (is_compl(i, j) && is_compl(i1, j1))
          if (i1 - i + j - j1 > 2 && i1 - i + j - j1 <= 30) {
            if (VBI[i][j] > getBulgeOrLoop(i, j, i1, j1) + V[i1][j1]) {
              path[i][j].set_bulge_loop(i1, j1);
              VBI[i][j] = getBulgeOrLoop(i, j, i1, j1) + V[i1][j1];
            }
            
          }
        }
      }
    }

    double get_bl_value(size_t i, size_t j) {
      if (!path[i][j].is_i1j1_set())
        return EnergiesInfo::INF;
      return getBulgeOrLoop(i, j, path[i][j].i1, path[i][j].j1) + V[path[i][j].i1][path[i][j].j1];
    }

    double get_vm_value(size_t i, size_t j) {
      if (!path[i][j].is_k1_set())
        return EnergiesInfo::INF;
      return W[i + 1][path[i][j].k1] + W[path[i][j].k1 + 1][j - 1];
    }

    void calculate_V_at(std::string const& str, size_t i, size_t j) {
      V[i][j] = EnergiesInfo::INF;
      if (is_compl(i, j)) {
        if (V[i][j] > energies_info.get(EnergiesInfo::Structure::Hairpin, j - i)) {
          path[i][j].set_hairpin();
          V[i][j] = energies_info.get(EnergiesInfo::Structure::Hairpin, j - i);
        }  
      }

      if (is_compl(i, j)) {
        if (V[i][j] > energies_info.get(str[i], str[j], str[i + 1], str[j - 1]) + V[i + 1][j - 1]) {
          path[i][j].set_stacked_status();
          V[i][j] = energies_info.get(str[i], str[j], str[i + 1], str[j - 1]) + V[i + 1][j - 1];
        }
      }
      if (V[i][j] > VBI[i][j]) {
        path[i][j].set_bulge_loop();
        V[i][j] = std::min(V[i][j], VBI[i][j]);
      } 
      if (V[i][j] > VM[i][j]) {
        path[i][j].set_vm();
        V[i][j] = std::min(V[i][j], VM[i][j]);
      }
    }

    void calculate_W_at(std::string const & str, size_t i, size_t j) {
      W[i][j] = V[i][j];
      if (W[i][j] > W[i][j - 1]) {
        W[i][j] = W[i][j - 1]; 
        path[i][j].set_shift();
      }
      if (W[i][j] > W[i + 1][j]) {
        W[i][j] = W[i + 1][j];
        path[i][j].set_shift();
      }
      
      for (size_t k = i + 1; k < j; ++k) {
        if (W[i][j] > W[i][k] + W[k + 1][j]) {
          path[i][j].set_w(k);
          W[i][j] = W[i][k] + W[k + 1][j];
        }
      }
    }

    //helper method: calculates values in (i, j) cell for all matrices
    void calculate_at(std::string const& str, size_t i, size_t j) {
      calculate_VM_at(str, i, j);
      calculate_VBI_at(str, i, j);
      calculate_V_at(str, i, j);
      calculate_W_at(str, i, j);
    }

   
    //method forms sequences
    std::vector<StackedSeqPair> to_sequences(std::vector<PairedCoords> & backtracing_coordinates, std::string const & str) {
        std::vector<StackedSeqPair> res;
        backtracing_coordinates.erase(
            std::unique(backtracing_coordinates.begin(), backtracing_coordinates.end()),
            backtracing_coordinates.end()
        );

      if (backtracing_coordinates.size() < 1)
          return res;

      PairedCoords begin = backtracing_coordinates[0], 
                    end = backtracing_coordinates[0];
      size_t length = 1;
      for (size_t i = 1; i < backtracing_coordinates.size(); i++) {
          if (!(backtracing_coordinates[i].first == backtracing_coordinates[i - 1].first + 1
            && backtracing_coordinates[i].second + 1 == backtracing_coordinates[i - 1].second)) {
              //print part of path if length > 0
              if (length > 0) {
                res.push_back(
                    std::make_pair(std::make_pair(begin.first, str.substr(begin.first, length)),
                    std::make_pair(end.second, str.substr(end.second, length)))
                    );
                begin = backtracing_coordinates[i];
              }
              //
              length = 1;
          } else length ++;
          end = backtracing_coordinates[i];
      }
      if (length > 0)
        res.push_back(
              std::make_pair(std::make_pair(begin.first, str.substr(begin.first, length)),
              std::make_pair(end.second, str.substr(end.second, length)))
              );

      return res;
    }

  public:

    std::vector<StackedSeqPair> get_result(size_t current_i, size_t current_j, std::string const& str) {
      std::vector<StackedSeqPair> result;
      std::vector<PairedCoords> backtracing_coordinates;

      if (str.empty())
        return result;

      size_t i = current_i,
             j = current_j;
      bool last_stacked = false;
      
      while (i < j && W[i][j] < EnergiesInfo::INF) {
        if (path[i][j].opt_structure == EnergiesInfo::Structure::Hairpin || 
                (last_stacked && V[i][j] == energies_info.get(EnergiesInfo::Structure::Hairpin, j - i))) {
            backtracing_coordinates.push_back(std::make_pair(i, j));
            break;
        }
        if (path[i][j].opt_structure == EnergiesInfo::Structure::Stacked || 
                (last_stacked && V[i][j] == energies_info.get(str[i], str[j], str[i + 1], str[j - 1]) + V[i + 1][j - 1])) {
            backtracing_coordinates.push_back(std::make_pair(i, j));
            backtracing_coordinates.push_back(std::make_pair(i + 1, j - 1));
            last_stacked = true;
            i = i + 1;
            j = j - 1;
            continue;
        }
        // bulgeorloop
        if (path[i][j].opt_structure == EnergiesInfo::Structure::BulgeOrLoop || (last_stacked && V[i][j] == get_bl_value(i, j))) {
            last_stacked = true;
            backtracing_coordinates.push_back(std::make_pair(i, j));
            backtracing_coordinates.push_back(std::make_pair(path[i][j].i1, path[i][j].j1));
            size_t i_old = i;
            i = path[i][j].i1;
            j = path[i_old][j].j1;
            continue;
        }
        if (path[i][j].opt_structure == EnergiesInfo::Structure::ComposedVM || (last_stacked && V[i][j] == get_vm_value(i, j))) {
            backtracing_coordinates.push_back(std::make_pair(i, j));
            return load_parts(backtracing_coordinates, str, i + 1, j - 1);
        }

        if (path[i][j].opt_structure == EnergiesInfo::Structure::Shift) {
            last_stacked = false;
            if (W[i][j] == W[i + 1][j])
                i = i + 1;
            else if (W[i][j] == W[i][j - 1]) 
                j = j - 1;
            continue;
        }
        if (W[i][j] != V[i][j] && path[i][j].opt_structure == EnergiesInfo::Structure::Composed) {
          
          return load_parts(backtracing_coordinates, str, i, j);
        }
      }
      ////after main loop, something may be unprocessed
      std::vector<StackedSeqPair> res1 = to_sequences(backtracing_coordinates, str);
      result.insert(result.begin(), res1.begin(), res1.end());
      return result;
    }

    // helper method, hides recursion
    // goes to i+1, k and k+1, j, merge result to current, returns result;
    std::vector<StackedSeqPair > load_parts(std::vector<PairedCoords> & coordinates, std::string const& str, size_t i, size_t j) {
				std::vector<StackedSeqPair > result = to_sequences(coordinates, str);
				std::vector<StackedSeqPair > res1 = get_result(i, path[i][j].k1, str);
				result.insert(result.begin(), res1.begin(), res1.end());
				res1 = get_result(path[i][j].k1 + 1, j, str);
				result.insert(result.begin(), res1.begin(), res1.end());
				return result;
    }

    bool is_complement(char a, char b) const {
      if (a > b)
        return is_complement(b, a);
      if ((a == 'A' || a == 'a') && (b == 'U' || b == 'u'))
        return true;
      if ((a == 'C' || a == 'c') && (b == 'G' || b == 'g'))
        return true;
      return false;
    }
   protected:
     bool is_compl(size_t i, size_t j) const {
       return is_complement(processed_str[i], processed_str[j]);
     }
  private:
    EnergiesInfo const& energies_info;
    size_t string_size;
    std::vector<std::vector<double> > W, V, VBI, VM;//matrices for recursion
    std::vector<std::vector<data_element> > path;
    double _a;
    std::string processed_str;
};
