#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include "energies_info.h"


class RNAEnergiesPredictor {
  public:
    RNAEnergiesPredictor(EnergiesInfo const& ei): energies_info(ei), _a(1) {
    }

    void ProcessSeq(std::string const& str) {
      string_size = str.size();
      prepare_data();
      for (int i = string_size - 2; i >= 0; i--) {
        for (int j = i + 1; j < string_size; j++) {
          calculate_at(str, i, j);
        }
      }
    }

    double getEnergy()const {
      return W[0][string_size - 1];
    }
  protected:
    void prepare_data() {
      W.resize(string_size, std::vector<double>(string_size, 0));
      V.resize(string_size, std::vector<double>(string_size, 0));
      VBI.resize(string_size, std::vector<double>(string_size, 0));
      VM.resize(string_size, std::vector<double>(string_size, 0));
      //initialize W and V with some infinite values
      for (int i = 0; i < string_size; i ++ ) {
        for (int j = i + 1; j - 4 < i && j < string_size; j++) {
          W[i][j] = EnergiesInfo::INF;
          V[i][j] = EnergiesInfo::INF;
        }
      }
    }

    //helper method: calculates VM values in (i,j) cell 
    void calculate_VM_at(std::string const& str, int i, int j) {
      //calculate VM
      VM[i][j] = EnergiesInfo::INF;
      for (int k = i + 1; k < j - 1; k++) { 
        VM[i][j] = std::min(VM[i][j], W[i + 1][k] + W[k + 1][j - 1]);
      }
      VM[i][j] += _a;
    }

    double getBulgeOrLoop(int i, int j, int i1, int j1) {
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
          if (i1 - i + j - j1 > 2) 
            VBI[i][j] = std::min(VBI[i][j], getBulgeOrLoop(i, j, i1, j1) + V[i1][j1]);
        }
      }
    }

    void calculate_V_at(std::string const& str, int i, int j) {
      V[i][j] = EnergiesInfo::INF;
      V[i][j] = std::min(V[i][j], energies_info.get(EnergiesInfo::Structure::Hairpin, j - i));
      V[i][j] = std::min(V[i][j], energies_info.get(str[i], str[j], str[i + 1], str[j - 1]) + V[i + 1][j - 1]);
      V[i][j] = std::min(V[i][j], VBI[i][j]);
      V[i][j] = std::min(V[i][j], VM[i][j]);
    }

    void calculate_W_at(std::string const & str, int i, int j) {
      W[i][j] = std::min(W[i][j - 1], W[i + 1][j]);
      W[i][j] = std::min(W[i][j], V[i][j]);
      for (int k = i + 1; k < j; ++k) {
        W[i][j] = std::min(W[i][j], W[i][k] + W[k + 1][j]);
      }
    }

    //helper method: calculates values in (i,j) cell for all matrices
    void calculate_at(std::string const& str, int i, int j) {
      calculate_VM_at(str, i, j);
      calculate_VBI_at(str, i, j);
      calculate_V_at(str, i, j);
      calculate_W_at(str, i, j);
    }
  private:
    EnergiesInfo const& energies_info;
    size_t string_size;
    std::vector<std::vector<double> > W, V, VBI, VM;//matrices for recursion
    double _a;
};