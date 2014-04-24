#pragma once
#include <vector>
#include <string>
#include "scoring.h"

struct Simple4dAligner {
  std::vector<std::string> * data;
  Scoring * scoring;
  std::vector< 
    std::vector< 
      std::vector<
        std::vector<int> > > > matrix;
  int string_size1, string_size2, string_size3, string_size4;

  Simple4dAligner(std::vector<std::string> * d, Scoring *s): data(d), scoring(s) {
    string_size1 = d->at(0).size();
    string_size2 = d->at(1).size();
    string_size3 = d->at(2).size();
    string_size4 = d->at(3).size();

    matrix.resize(string_size1 + 1);
    for (int i = 0; i < string_size1 + 1; i ++) {
      matrix[i].resize(string_size2 + 1);
      for (int j = 0; j < string_size2 + 1; j++) {
        matrix[i][j].resize(string_size3 + 1);
        for (int k = 0 ; k < string_size3 + 1; k++) {
          matrix[i][j][k].resize(string_size4 + 1, 0);
        }
      }
    }
    initialize();
  }

  void initialize() {
    for (int i = 0; i < string_size1; i++) {
      matrix[i + 1][0][0][0] = matrix[i][0][0][0] + delta(data->at(0)[i], '-', '-', '-');
    }
    for (int i = 0; i < string_size2; i++) {
      matrix[0][i + 1][0][0] = matrix[0][i][0][0] + delta(data->at(1)[i], '-', '-', '-');
    }
    for (int i = 0; i < string_size3; i++) {
      matrix[0][0][i + 1][0] = matrix[0][0][i][0] + delta(data->at(2)[i], '-', '-', '-');
    }
    for (int i = 0; i < string_size4; i++) {
      matrix[0][0][0][i + 1] = matrix[0][0][0][i] + delta(data->at(3)[i], '-', '-', '-');
    }
  }

  void align2() {
    for (int i = 0; i < string_size1; i++) {
      for (int j = 0; j < string_size2; j++) {
        matrix[i + 1][j + 1][0][0] = std::min(matrix[i][j][0][0] + delta(data->at(0)[i], data->at(1)[j], '-', '-'), 
            std::min(
              matrix[i][j + 1][0][0] + delta('-', data->at(1)[j], '-', '-'), 
              matrix[i + 1][j][0][0] + delta(data->at(0)[i], '-', '-', '-')
              ));
      }
      for (int j = 0; j < string_size3; j++) {
        matrix[i + 1][0][j + 1][0] = std::min(matrix[i][0][j][0] + delta(data->at(0)[i], data->at(2)[j], '-', '-'), 
          std::min(
            matrix[i][0][j + 1][0] + delta('-', data->at(2)[j], '-', '-'), 
            matrix[i + 1][0][j][0] + delta(data->at(0)[i], '-', '-', '-')
            ));
      }      
      for (int j = 0; j < string_size4; j++) {
        matrix[i + 1][0][0][j + 1] = std::min(matrix[i][0][0][j] + delta(data->at(0)[i], data->at(3)[j], '-', '-'), 
          std::min(
            matrix[i][0][0][j + 1] + delta('-', data->at(3)[j], '-', '-'), 
            matrix[i + 1][0][0][j] + delta(data->at(0)[i], '-', '-', '-')
            ));
      }
    }

    //


    for (int i = 0; i < string_size2; i++) {
      for (int j = 0; j < string_size3; j++) {

        matrix[0][i + 1][j + 1][0] = std::min(matrix[0][i][j][0] + delta(data->at(1)[i], data->at(2)[j], '-', '-'), 
          std::min(
            matrix[0][i][j + 1][0] + delta('-', data->at(2)[j], '-', '-'), 
            matrix[0][i + 1][j][0] + delta(data->at(1)[i], '-', '-', '-')
            ));
      }
      for (int j = 0; j < string_size4; j++) {

        matrix[0][i + 1][0][j + 1] = std::min(matrix[0][i][0][j] + delta(data->at(1)[i], data->at(3)[j], '-', '-'), 
          std::min(
            matrix[0][i][0][j + 1] + delta('-', data->at(3)[j], '-', '-'), 
            matrix[0][i + 1][0][j] + delta(data->at(1)[i], '-', '-', '-')
            ));
      }
    }

    for (int i = 0; i < string_size3; i++) {
      for (int j = 0; j < string_size4; j++) {
        matrix[0][0][i + 1][j + 1] = std::min(matrix[0][0][i][j] + delta(data->at(2)[i], data->at(3)[j], '-', '-'), 
          std::min(
            matrix[0][0][i][j + 1] + delta('-', data->at(3)[j], '-', '-'), 
            matrix[0][0][i + 1][j] + delta(data->at(2)[i], '-', '-', '-')
            ));
      }
    }
  }

  int delta1(char a, char b, char c) {
    return scoring->getScore(a, b) + scoring->getScore(b, c) + scoring->getScore(a, c);
  }
  int delta(char a, char b, char c, char d) {
    return scoring->getScore(a, b) + scoring->getScore(a, c) + scoring->getScore(a, d) + delta1(b, c, d);
  }

  void align3() {
    for (int i = 0; i < string_size1; i ++) {
      for (int j = 0; j < string_size2; j ++) {
        for (int k = 0; k < string_size3; k ++) {
          //4:
          matrix[i + 1][j + 1][k + 1][0] = std::min(
              matrix[i][j][k][0] + delta(data->at(0)[i], data->at(1)[j], data ->at(2)[k], '-'),
              matrix[i + 1][j][k][0] + delta('-', data->at(1)[j], data ->at(2)[k], '-')
            );
          matrix[i + 1][j + 1][k + 1][0] = std::min(
              matrix[i + 1][j + 1][k + 1][0],
              matrix[i][j + 1][k][0] + delta(data->at(0)[i], '-', data ->at(2)[k], '-')
            );
          matrix[i + 1][j + 1][k + 1][0] = std::min(
              matrix[i + 1][j + 1][k + 1][0],
              matrix[i][j][k + 1][0] + delta(data->at(0)[i], data->at(1)[j], '-', '-')
            );
          matrix[i + 1][j + 1][k + 1][0] = std::min(
              matrix[i + 1][j + 1][k + 1][0],
              matrix[i + 1][j + 1][k][0] + delta('-', '-', data ->at(2)[k], '-')
            );
          matrix[i + 1][j + 1][k + 1][0] = std::min(
              matrix[i + 1][j + 1][k + 1][0],
              matrix[i + 1][j][k + 1][0] + delta('-', data->at(1)[j], '-', '-')
            );
          matrix[i + 1][j + 1][k + 1][0] = std::min(
              matrix[i + 1][j + 1][k + 1][0],
              matrix[i][j + 1][k + 1][0] + delta(data->at(0)[i], '-', '-', '-')
            );
        }
      }
    }
    for (int i = 0; i < string_size1; i ++) {
      for (int j = 0; j < string_size2; j ++) {
        for (int k = 0; k < string_size4; k ++) {

          //3:
          matrix[i + 1][j + 1][0][k + 1] = std::min(
              matrix[i][j][0][k] + delta(data->at(0)[i], data->at(1)[j], data ->at(3)[k], '-'),
              matrix[i + 1][j][0][k] + delta('-', data->at(1)[j], data ->at(3)[k], '-')
            );
          matrix[i + 1][j + 1][0][k + 1] = std::min(
              matrix[i + 1][j + 1][0][k + 1],
              matrix[i][j + 1][0][k] + delta(data->at(0)[i], '-', data ->at(3)[k], '-')
            );
          matrix[i + 1][j + 1][0][k + 1] = std::min(
              matrix[i + 1][j + 1][0][k + 1],
              matrix[i][j][0][k + 1] + delta(data->at(0)[i], data->at(1)[j], '-', '-')
            );
          matrix[i + 1][j + 1][0][k + 1] = std::min(
              matrix[i + 1][j + 1][0][k + 1],
              matrix[i + 1][j + 1][0][k] + delta('-', '-', data ->at(3)[k], '-')
            );
          matrix[i + 1][j + 1][0][k + 1] = std::min(
              matrix[i + 1][j + 1][0][k + 1],
              matrix[i + 1][j][0][k + 1] + delta('-', data->at(1)[j], '-', '-')
            );
          matrix[i + 1][j + 1][0][k + 1] = std::min(
              matrix[i + 1][j + 1][0][k + 1],
              matrix[i][j + 1][0][k + 1] + delta(data->at(0)[i], '-', '-', '-')
            );
        }
      }
      //2:
      for (int j = 0; j < string_size3; j ++) {
        for (int k = 0; k < string_size4; k ++) {
          //2:
          matrix[i + 1][0][j + 1][k + 1] = std::min(
              matrix[i][0][j][k] + delta(data->at(0)[i], data->at(2)[j], data ->at(3)[k], '-'),
              matrix[i + 1][0][j][k] + delta('-', data->at(2)[j], data ->at(3)[k], '-')
            );
          matrix[i + 1][0][j + 1][k + 1] = std::min(
              matrix[i + 1][0][j + 1][k + 1],
              matrix[i][0][j + 1][k] + delta(data->at(0)[i], '-', data ->at(3)[k], '-')
            );
          matrix[i + 1][0][j + 1][k + 1] = std::min(
              matrix[i + 1][0][j + 1][k + 1],
              matrix[i][0][j][k + 1] + delta(data->at(0)[i], data->at(2)[j], '-', '-')
            );
          matrix[i + 1][0][j + 1][k + 1] = std::min(
              matrix[i + 1][0][j + 1][k + 1],
              matrix[i + 1][0][j + 1][k] + delta('-', '-', data ->at(3)[k], '-')
            );
          matrix[i + 1][0][j + 1][k + 1] = std::min(
              matrix[i + 1][0][j + 1][k + 1],
              matrix[i + 1][0][j][k + 1] + delta('-', data->at(2)[j], '-', '-')
            );
          matrix[i + 1][0][j + 1][k + 1] = std::min(
              matrix[i + 1][0][j + 1][k + 1],
              matrix[i][0][j + 1][k + 1] + delta(data->at(0)[i], '-', '-', '-')
            );
        }
      }
    }
    for (int i = 0; i < string_size2; i ++) {
      for (int j = 0; j < string_size3; j ++) {
        for (int k = 0; k < string_size4; k ++) {

          //1:
          matrix[0][i + 1][j + 1][k + 1] = std::min(
              matrix[0][i][j][k] + delta(data->at(1)[i], data->at(2)[j], data ->at(3)[k], '-'),
              matrix[0][i + 1][j][k] + delta('-', data->at(2)[j], data ->at(3)[k], '-')
            );
          matrix[0][i + 1][j + 1][k + 1] = std::min(
              matrix[0][i + 1][j + 1][k + 1],
              matrix[0][i][j + 1][k] + delta(data->at(1)[i], '-', data ->at(3)[k], '-')
            );
          matrix[0][i + 1][j + 1][k + 1] = std::min(
              matrix[0][i + 1][j + 1][k + 1],
              matrix[0][i][j][k + 1] + delta(data->at(1)[i], data->at(2)[j], '-', '-')
            );
          matrix[0][i + 1][j + 1][k + 1] = std::min(
              matrix[0][i + 1][j + 1][k + 1],
              matrix[0][i + 1][j + 1][k] + delta('-', '-', data ->at(3)[k], '-')
            );
          matrix[0][i + 1][j + 1][k + 1] = std::min(
              matrix[0][i + 1][j + 1][k + 1],
              matrix[0][i + 1][j][k + 1] + delta('-', data->at(2)[j], '-', '-')
            );
          matrix[i + 1][0][j + 1][k + 1] = std::min(
              matrix[0][i + 1][j + 1][k + 1],
              matrix[0][i][j + 1][k + 1] + delta(data->at(1)[i], '-', '-', '-')
            );

        }
      }
    }
  }

  void align4() {
    for (int i = 0; i < string_size1; i++) {
      for (int j = 0; j < string_size2; j++) {
        for (int k = 0; k < string_size3; k++) {
          for (int l = 0; l < string_size4; l++) {
            matrix[i + 1][j + 1][k + 1][l + 1] = matrix[i][j][k][l] + delta(data->at(0)[i], data->at(1)[j], data->at(2)[k], data->at(3)[l]);
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i + 1][j][k][l] + delta('-', data->at(1)[j], data->at(2)[k], data->at(3)[l]));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i][j + 1][k][l] + delta(data->at(0)[i], '-', data->at(2)[k], data->at(3)[l]));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i][j][k + 1][l] + delta(data->at(0)[i], data->at(1)[j], '-', data->at(3)[l]));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i][j][k][l + 1] + delta(data->at(0)[i], data->at(1)[j], data->at(2)[k], '-'));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i + 1][j + 1][k][l] + delta('-', '-', data->at(2)[k], data->at(3)[l]));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i + 1][j][k + 1][l] + delta('-', data->at(1)[j], '-', data->at(3)[l]));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i + 1][j][k][l + 1] + delta('-', data->at(1)[j], data->at(2)[k], '-'));

            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i][j + 1][k + 1][l] + delta(data->at(0)[i], '-', '-', data->at(3)[l]));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i][j + 1][k][l + 1] + delta(data->at(0)[i], '-', data->at(2)[k], '-'));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i][j][k + 1][l + 1] + delta(data->at(0)[i], data->at(1)[j], '-', '-'));

            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i + 1][j + 1][k + 1][l] + delta('-', '-', '-', data->at(3)[l]));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i + 1][j + 1][k][l + 1] + delta('-', '-', data->at(2)[k], '-'));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i + 1][j][k + 1][l + 1] + delta('-', data->at(1)[j], '-', '-'));
            matrix[i + 1][j + 1][k + 1][l + 1] = std::min( matrix[i + 1][j + 1][k + 1][l + 1],
              matrix[i][j + 1][k + 1][l + 1] + delta(data->at(0)[i], '-', '-', '-'));
          }
        }
      }
    }
  }

  void Align() {
    align2();
    align3();
    align4();
  }

  std::vector<std::string> ReconstructAlignment() {
    std::vector<std::string> alignment;
    alignment.resize(4, "");
    int i = string_size1 - 1;
    int j = string_size2 - 1;
    int k = string_size3 - 1;
    int l = string_size4 - 1;

    //
    //
    while ( i + 1 > 0 || j + 1 > 0 || k + 1 > 0 || l + 1 > 0) {

      if (i >= 0 && j >= 0 && k >= 0 && l >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i][j][k][l] + delta(data->at(0)[i], data->at(1)[j], data->at(2)[k], data->at(3)[l])) {
        alignment[0].push_back(data->at(0)[i]);
        alignment[1].push_back(data->at(1)[j]);
        alignment[2].push_back(data->at(2)[k]);
        alignment[3].push_back(data->at(3)[l]);
        i = i - 1;
        j = j - 1;
        k = k - 1;
        l = l - 1;
      }
      if (j >= 0 && k >= 0 && l >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i + 1][j][k][l] + delta('-', data->at(1)[j], data->at(2)[k], data->at(3)[l])) {
        alignment[0].push_back('-');
        alignment[1].push_back(data->at(1)[j]);
        alignment[2].push_back(data->at(2)[k]);
        alignment[3].push_back(data->at(3)[l]);
        j = j - 1;
        k = k - 1;
        l = l - 1;
      }
      if (i >= 0 && k >= 0 && l >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i][j + 1][k][l] + delta(data->at(0)[i], '-', data->at(2)[k], data->at(3)[l])) {
        alignment[0].push_back(data->at(0)[i]);
        alignment[1].push_back('-');
        alignment[2].push_back(data->at(2)[k]);
        alignment[3].push_back(data->at(3)[l]);
        i = i - 1;
        k = k - 1;
        l = l - 1;
      }
      if (i >= 0 && j >= 0 && l >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i][j][k + 1][l] + delta(data->at(0)[i], data->at(1)[j], '-', data->at(3)[l])) {
        alignment[0].push_back(data->at(0)[i]);
        alignment[1].push_back(data->at(1)[j]);
        alignment[2].push_back('-');
        alignment[3].push_back(data->at(3)[l]);
        i = i - 1;
        j = j - 1;
        l = l - 1;
      }
      if (i >= 0 && j >= 0 && k >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i][j][k][l + 1] + delta(data->at(0)[i], data->at(1)[j], data->at(2)[k], '-')) {
        alignment[0].push_back(data->at(0)[i]);
        alignment[1].push_back(data->at(1)[j]);
        alignment[2].push_back(data->at(2)[k]);
        alignment[3].push_back('-');
        i = i - 1;
        j = j - 1;
        k = k - 1;
      }
      if (k >= 0 && l >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i + 1][j + 1][k][l] + delta('-', '-', data->at(2)[k], data->at(3)[l])) {
        alignment[0].push_back('-');
        alignment[1].push_back('-');
        alignment[2].push_back(data->at(2)[k]);
        alignment[3].push_back(data->at(3)[l]);
        k = k - 1;
        l = l - 1;
      }
      if (j >= 0 && l >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i + 1][j][k + 1][l] + delta('-', data->at(1)[j], '-', data->at(3)[l])) {
        alignment[0].push_back('-');
        alignment[1].push_back(data->at(1)[j]);
        alignment[2].push_back('-');
        alignment[3].push_back(data->at(3)[l]);
        j = j - 1;
        l = l - 1;
      }
      if (j >= 0 && k >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i + 1][j][k][l + 1] + delta('-', data->at(1)[j], data->at(2)[k], '-')) {
        alignment[0].push_back('-');
        alignment[1].push_back(data->at(1)[j]);
        alignment[2].push_back(data->at(2)[k]);
        alignment[3].push_back('-');
        j = j - 1;
        k = k - 1;
      }
      if (i >= 0 && l >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i][j + 1][k + 1][l] + delta(data->at(0)[i], '-', '-', data->at(3)[l])) {
        alignment[0].push_back(data->at(0)[i]);
        alignment[1].push_back('-');
        alignment[2].push_back('-');
        alignment[3].push_back(data->at(3)[l]);
        i = i - 1;
        l = l - 1;
      }
      if (i >= 0 && k >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i][j + 1][k][l + 1] + delta(data->at(0)[i], '-', data->at(2)[k], '-')) {
        alignment[0].push_back(data->at(0)[i]);
        alignment[1].push_back('-');
        alignment[2].push_back(data->at(2)[k]);
        alignment[3].push_back('-');
        i = i - 1;
        k = k - 1;
      }
      if (i >= 0 && j >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i][j][k + 1][l + 1] + delta(data->at(0)[i], data->at(1)[j], '-', '-')) {
        alignment[0].push_back(data->at(0)[i]);
        alignment[1].push_back(data->at(1)[j]);
        alignment[2].push_back('-');
        alignment[3].push_back('-');
        i = i - 1;
        j = j - 1;
      }
      if (l >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i + 1][j + 1][k + 1][l] + delta('-', '-', '-', data->at(3)[l])) {
        alignment[0].push_back('-');
        alignment[1].push_back('-');
        alignment[2].push_back('-');
        alignment[3].push_back(data->at(3)[l]);
        l = l - 1;
      }
      if (k >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i + 1][j + 1][k][l + 1] + delta('-', '-', data->at(2)[k], '-')) {
        alignment[0].push_back('-');
        alignment[1].push_back('-');
        alignment[2].push_back(data->at(2)[k]);
        alignment[3].push_back('-');
        k = k - 1;
      }
      if (j >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i + 1][j][k + 1][l + 1] + delta('-', data->at(1)[j], '-', '-')) {
        alignment[0].push_back('-');
        alignment[1].push_back(data->at(1)[j]);
        alignment[2].push_back('-');
        alignment[3].push_back('-');
        j = j - 1;
      }
      if (i >= 0 && matrix[i + 1][j + 1][k + 1][l + 1] == matrix[i][j + 1][k + 1][l + 1] + delta(data->at(0)[i], '-', '-', '-')) {
        alignment[0].push_back(data->at(0)[i]);
        alignment[1].push_back('-');
        alignment[2].push_back('-');
        alignment[3].push_back('-');
        i = i - 1 ;
      }
    }
    for (int i = 0; i < 4; i ++) {
      std::reverse(alignment[i].begin(), alignment[i].end());
    }
    return alignment;
    //
  }
};