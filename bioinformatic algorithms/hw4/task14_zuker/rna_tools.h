#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include "energies_info.h"

typedef double ValueType;
typedef std::pair<size_t,std::string> ResultElType;

struct data_element{
  data_element(): opt_structure(EnergiesInfo::Structure::None) {
  }

  void set_bulge_loop(size_t i, size_t j) {
    opt_structure = EnergiesInfo::Structure::BulgeOrLoop;
    i1 = i;
    j1 = j;
  }

  void set_w(size_t k) {
    opt_structure = EnergiesInfo::Structure::Composed;
    k1 = k;
  }
  void set_w0() {
    opt_structure = EnergiesInfo::Structure::Simple;
  }
  void set_vm(size_t k) {
    opt_structure = EnergiesInfo::Structure::ComposedVM;
    k1 = k;
  }
  void set_hairpin() {
    opt_structure = EnergiesInfo::Structure::Hairpin;
  }

  void set_stack() {
    opt_structure = EnergiesInfo::Structure::Hairpin;
  }
  void clear() {
    opt_structure = EnergiesInfo::Structure::None;
  }

  EnergiesInfo::Structure opt_structure;
  int i1, j1, k1;
};


class RNAEnergiesPredictor {
  public:
    RNAEnergiesPredictor(EnergiesInfo const& ei): energies_info(ei), _a(1) {
    }

    void ProcessSeq(std::string const& str) {
      processed_str = str;
      string_size = str.size();
      prepare_data();
      for (int i = string_size - 2; i >= 0; i--) {
        for (int j = i + 1; j < string_size; j++) {
          //std::cout << i << " " << j << std::endl;
          calculate_at(str, i, j);
        }
      }
    }

    double getEnergy()const {
      return W[0][string_size - 1];
    }
  protected:
    void prepare_data() {
      W.resize(string_size, std::vector<double>(string_size, EnergiesInfo::INF));
      V.resize(string_size, std::vector<double>(string_size, EnergiesInfo::INF));
      VBI.resize(string_size, std::vector<double>(string_size, 0));
      VM.resize(string_size, std::vector<double>(string_size, 0));
      path.resize(string_size, std::vector<data_element>(string_size));

      //initialize W and V with some infinite values
      /*for (int i = 0; i < string_size; i ++ ) {
        for (int j = i + 1; j - 4 < i && j < string_size; j++) {
          W[i][j] = EnergiesInfo::INF;
          V[i][j] = EnergiesInfo::INF;
        }
      }*/
    }

    //helper method: calculates VM values in (i,j) cell 
    void calculate_VM_at(std::string const& str, int i, int j) {
      //calculate VM
      VM[i][j] = EnergiesInfo::INF;
      for (int k = i + 1; k < j - 1; k++) { 
        if (VM[i][j] > W[i + 1][k] + W[k + 1][j - 1]) {
          path[i][j].set_vm(k);
        }
        VM[i][j] = std::min(VM[i][j], W[i + 1][k] + W[k + 1][j - 1]);
      }
      VM[i][j] += _a;
    }

    double getBulgeOrLoop(int i, int j, int i1, int j1) {
      if (!(is_compl(i1, j1) && is_compl(i, j)))
          return EnergiesInfo::INF;
      if (j - j1 == 1 || i1 - i == 1) {
        return energies_info.get(EnergiesInfo::Structure::Bulge, std::max(j - j1 - 1, i1 - i - 1));
      }
      else {
        return energies_info.get(EnergiesInfo::Structure::InternalLoop, j - j1 - 1 + i1 - i - 1);
      }
    }

    void calculate_VBI_at(std::string const& str, int i, int j) {
      VBI[i][j] = EnergiesInfo::INF;
      for (int i1 = i + 1; i1 < j; i1++) { 
        for (int j1 = i1 + 1; j1 < j; j1++) {
          if (i1 - i + j - j1 > 2 && i1 - i + j - j1 <= 30) {
            if (VBI[i][j] > getBulgeOrLoop(i, j, i1, j1) + V[i1][j1]) {
              path[i][j].set_bulge_loop(i1, j1);
            }
            VBI[i][j] = std::min(VBI[i][j], getBulgeOrLoop(i, j, i1, j1) + V[i1][j1]);
          }
        }
      }
    }

    void calculate_V_at(std::string const& str, int i, int j) {
      V[i][j] = EnergiesInfo::INF;
      if (is_compl(i, j)) {
        if (V[i][j] > energies_info.get(EnergiesInfo::Structure::Hairpin, j - i)) {
          path[i][j].set_hairpin();
        }
        V[i][j] = std::min(V[i][j], energies_info.get(EnergiesInfo::Structure::Hairpin, j - i));
      }
      if (is_compl(i, j) && is_compl(i + 1, j - 1)){
        if (V[i][j] > energies_info.get(str[i], str[j], str[i + 1], str[j - 1]) + V[i + 1][j - 1]) {
          path[i][j].set_stack();
        }
        V[i][j] = std::min(V[i][j], energies_info.get(str[i], str[j], str[i + 1], str[j - 1]) + V[i + 1][j - 1]);
      }
      V[i][j] = std::min(V[i][j], VBI[i][j]);
      V[i][j] = std::min(V[i][j], VM[i][j]);
    }

    void calculate_W_at(std::string const & str, int i, int j) {
      W[i][j] = std::min(W[i][j - 1], W[i + 1][j]);
      if (W[i][j] < V[i][j])
        path[i][j].set_w0();
      W[i][j] = std::min(W[i][j], V[i][j]);
      for (int k = i + 1; k < j; ++k) {
        if (W[i][j] > W[i][k] + W[k + 1][j])
          path[i][j].set_w(k);
        W[i][j] = std::min(W[i][j], W[i][k] + W[k + 1][j]);
      }
    }

    //helper method: calculates values in (i, j) cell for all matrices
    void calculate_at(std::string const& str, int i, int j) {
      calculate_VM_at(str, i, j);
      calculate_VBI_at(str, i, j);
      calculate_V_at(str, i, j);
      calculate_W_at(str, i, j);
    }

    std::vector<std::pair<ResultElType, ResultElType> > to_sequences(std::vector<std::pair<size_t, size_t> > & backtracing_coordinates, std::string const& str) {
      std::vector<std::pair<ResultElType, ResultElType> > res;
      backtracing_coordinates.erase(
        std::unique(backtracing_coordinates.begin(), backtracing_coordinates.end()),
        backtracing_coordinates.end()
        );
      if (backtracing_coordinates.size() < 1)
        return res;
      std::pair<size_t, size_t> begin = backtracing_coordinates[0], end = backtracing_coordinates[0];
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
            length = 0;
        } else length ++;
        end = backtracing_coordinates[i];
        
        //length ++;
      }
      if (length > 0)
        res.push_back(
              std::make_pair(std::make_pair(begin.first, str.substr(begin.first, length)), 
              std::make_pair(end.second, str.substr(end.second, length)))
              );

      return res;
    }
  public:
    std::vector<std::pair<ResultElType, ResultElType> > get_result(size_t current_i, size_t current_j, std::string const& str) {
      std::vector<std::pair<ResultElType, ResultElType> > result;
      std::vector<std::pair<size_t, size_t> > backtracing_coordinates;
      
      if (str.size() == 0)
        return result;

      size_t i = current_i, 
             j = current_j,
             length = 0,
             start1 = -1, 
             start2 = -1;

      while (i < j && W[i][j] < EnergiesInfo::INF) {
        if (W[i][j] == W[i + 1][j]) {
          i = i + 1;
          continue;
        }
        if (W[i][j] == W[i][j - 1]) {
          j = j - 1;
          continue;
        }

        if (W[i][j] == V[i][j]) {
        //
        //in other cases i and j are paired
        
          if (V[i][j] == energies_info.get(EnergiesInfo::Structure::Hairpin, j - i)) {
            backtracing_coordinates.push_back(std::make_pair(i, j));
            break;
          }
          if (V[i][j] == energies_info.get(str[i], str[j], str[i + 1], str[j - 1]) + V[i + 1][j - 1]) {
            backtracing_coordinates.push_back(std::make_pair(i, j));
            backtracing_coordinates.push_back(std::make_pair(i + 1, j - 1));
            i = i + 1;
            j = j - 1;
            continue;
          }
          // bulgeorloop
          if (path[i][j].opt_structure == EnergiesInfo::Structure::BulgeOrLoop) {
            backtracing_coordinates.push_back(std::make_pair(i, j));
            backtracing_coordinates.push_back(std::make_pair(path[i][j].i1, path[i][j].j1));
            i = path[i][j].i1;
            j = path[i][j].j1;
          }
          if (path[i][j].opt_structure == EnergiesInfo::Structure::ComposedVM) {
            std::vector<std::pair<ResultElType, ResultElType> > res1 = to_sequences(backtracing_coordinates, str);
            result.insert(result.begin(), res1.begin(), res1.end());
            res1 = get_result(i + 1, path[i][j].k1, str);
            result.insert(result.begin(), res1.begin(), res1.end());
            res1 = get_result(path[i][j].k1 + 1, j - 1, str);
            result.insert(result.begin(), res1.begin(), res1.end());
            return result;
          }
        }
        if (path[i][j].opt_structure == EnergiesInfo::Structure::Composed) {
          // go to i+1, k and k+1, j, merge result to current, return result;
          std::vector<std::pair<ResultElType, ResultElType> > res1 = to_sequences(backtracing_coordinates, str);
          result.insert(result.begin(), res1.begin(), res1.end());
          res1 = get_result(i, path[i][j].k1, str);
          result.insert(result.begin(), res1.begin(), res1.end());
          res1 = get_result(path[i][j].k1 + 1, j, str);
          result.insert(result.begin(), res1.begin(), res1.end());
          return result;
        }
        //

      }
      ////after main loop, something may be unprocessed
      //if (length > 0) {//add base pair info
      //  result.push_back(std::make_pair(std::make_pair(start1, str.substr(start1, length)),
      //                                  std::make_pair(start2, str.substr(start2, length))
      //    ));
      //  // no need to clear temp variables
      //}
      std::vector<std::pair<ResultElType, ResultElType> > res1 = to_sequences(backtracing_coordinates, str);
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
     bool is_compl(int i, int j) const {
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