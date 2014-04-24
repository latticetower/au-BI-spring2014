#pragma once
#include <iostream>
#include <fstream>

/* this struct should contain basic scoring reading functionality*/
struct Scoring {
  int _gap; 
  int _scoring[4][4];

  Scoring() {  }

  bool loadFromFile(const char* filename) {
    std::ifstream input_file(filename);
    if (input_file.fail())
      return false;
    loadFromStream(input_file);
    input_file.close();
    return true;
  }

  bool loadFromStream(std::istream& istream) {
    istream >> _gap;
    istream >> _scoring[0][0] >> _scoring[0][1] >> _scoring[0][2] >> _scoring[0][3]
            >> _scoring[1][0] >> _scoring[1][1] >> _scoring[1][2] >> _scoring[1][3]
            >> _scoring[2][0] >> _scoring[2][1] >> _scoring[2][2] >> _scoring[2][3]
            >> _scoring[3][0] >> _scoring[3][1] >> _scoring[3][2] >> _scoring[3][3];
    return true;
  }

  inline int getIndex(char a) {
    switch (a) {
    case 'A':
    case 'a':
      return 0;
    case 'C':
    case 'c':
      return 1;
    case 'G':
    case 'g':
      return 2;
    case 'T':
    case 't':
      return 3;
    }
    return -1;
  }
  inline bool isNucleotide(char a) {
    return (a == 'A') || (a == 'C') || (a == 'G') || (a == 'T') ||
           (a == 'a') || (a == 'c') || (a == 'g') || (a == 't');
  }

  int getScore(char a, char b) {
    if (isNucleotide(a) && isNucleotide(b))
      return _scoring[getIndex(a)][getIndex(b)];
    //if (a == b)
    //  return 0;
    return _gap;
  }
  int getGap() {
    return _gap;
  }
};