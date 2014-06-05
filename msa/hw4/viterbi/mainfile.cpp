#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include "fasta_data.h"

long double pi1 = log(0.996),
       pi2 = log(0.004);
long double a11 = log(0.999),
       a12 = log(0.001),
       a21 = log(0.01),
       a22 = log(0.99);
long double e1_at = log(0.291),
       e1_cg = log(0.209),
       e2_at = log(0.169),
       e2_cg = log(0.331);

long double getE(int t, char c) {
  if (t == 1) {
    if (c == 'A' || c=='T')
      return e1_at;
    else return e1_cg;
  }
  else {
    if (c == 'A' || c == 'T')
      return e2_at;
    return e2_cg;
  }
}

std::vector<std::pair<int, int> > x_values;
std::vector<std::pair<long double, long double> > v_values;

std::pair<long double, long double> forward_init(char chr){
  x_values.clear();
  v_values.clear();
  //x_values.push_back(std::make_pair(0, 0));
  long double x1 = getE(1, chr)+pi1;
  long double x2 = getE(2, chr)+pi2;
  v_values.push_back(std::make_pair(x1, x2));
  x_values.push_back(std::make_pair(1, 2));
  return std::make_pair(x1, x2);
}

std::pair<long double, long double> forward_step(char chr, std::pair<long double, long double> v_prev) {
  long double v1, v2;
  int x1, x2;
  if (a11+v_prev.first < a21+v_prev.second) {
    v1 = a21+v_prev.second;
    x1 = 2;
  } else
  {
    v1 = a11+v_prev.first;
    x1 = 1;
  }
  if (a12+v_prev.first < a22+v_prev.second) {
    v2 = a22+v_prev.second;
    x2 = 2;
  } else
  {
    v2 = a12 + v_prev.first;
    x2 = 1;
  }
  x_values.push_back(std::make_pair(x1, x2));
  std::pair<long double, long double> res = std::make_pair(getE(1, chr)+v1, getE(2,chr)+v2);
  v_values.push_back(res);
  return res;
}

std::pair<int, int> backward_init(size_t max_size){
    size_t t = max_size - 1;
    long double v1 = v_values[t].first;
    long double v2 = v_values[t].second;
    //std::cout << "v= "<<v1 <<" "<<v2 <<std::endl;
    if (v1 > v2) {
      return std::make_pair(1, t);
    }
    else
      return std::make_pair(2, t);
}

int ptr(std::pair<int, int> x_next){
  return (x_next.first == 1 ? x_values[x_next.second].first : x_values[x_next.second].second);
}

std::pair<int, int> backward_step(std::pair<int, int> x_next) {
    int t_1 = x_next.second - 1;
    return std::make_pair(ptr(x_next), t_1);
}

void get_state_sequence(std::string const& str, std::string& target) {
  std::pair<long double, long double> val = forward_init(str[0]);
  //std::cout << "forw" << std::endl;
 // std::cout << val.first << " " << val.second <<std::endl;
  for (size_t i = 1; i < str.size(); i++) {
    val = forward_step(str[i], val);
    //std::cout << val.first << " " << val.second <<std::endl;
  }
  //x_values = []

  //std::string str2 = ""
  //std::cout <<"\n backward\n";
  std::pair<int, int> x = backward_init(str.size());
  //std::cout << x.first << " " <<x.second <<std::endl;
  target[str.size() - 1] = (x.first == 1? '1' : '2');
  for (size_t i = 1; i < str.size(); i++) {
    x = backward_step(x);
    //std::cout << x.first << " " <<x.second <<std::endl;
    target[x.second] = (x.first == 1? '1' : '2');
  }
  //std::cout << x_values.size() << " " <<v_values.size() << " " <<str.size() << " - " << "dddd";
  return ;
}

long long _state1 = 0;
long long _state2 = 0;
long _segment1 = 0;
long _segment2 = 0;

void update_transitions(std::string const & states_string) {
  // clear transitions
  a11 = 0.0;
  a12 = 0.0;
  a21 = 0.0;
  a22 = 0.0;
  //

  _state1 = 0;
  _state2 = 0;
  _segment1 = 0;
  _segment2 = 0;
char cur_segment = states_string[0];
  if (states_string[0] == '1') {
      _state1 += 1;
      _segment1 = 1;
  } else {
      _state2 += 1;
      _segment2 = 1;
  }
  // process states_string
  for (size_t i = 0; i < states_string.size() - 1; i++) {
      //states and segments
      if (cur_segment != states_string[i + 1]) {
          if (states_string[i + 1] == '1') {
              _segment1 += 1;
          }
          else {
              _segment2 += 1;
          }
          cur_segment = states_string[i+1];
      }
      //
      char state1 = states_string[i];
      char state2 = states_string[i + 1];
      if (state1 == '1') {
          if (state2 == '1') {
              a11 += 1;
              _state1 += 1;

          }
          else {
              a12 += 1;
              _state2 += 1;
          }
      }
      else {
          if (state2 == '1') {
              a21 += 1;
              _state1 += 1;
          }
          else {
              a22 += 1;
              _state2 += 1;
          }

      }
  }
  //std::cout << a11 << " " << a12 << std::endl;

  //std::cout << a21 << " " << a22 << std::endl;
  // normalize
  long double res = a11 + a12;
  if (res!= 0) {
      a11 = a11 / res;
      a12 = a12 / res;
  }
  res = a21 + a22;
  if (res != 0) {
      a21 = a21 / res;
      a22 = a22 / res;
  }

  //std::cout<< " after "<< a11 << " " << a12 << std::endl;

  //std::cout << a21 << " " << a22 << std::endl;
}

void print_islands(std::ostream & os, std::string const & sequence, std::string const & statestr, const char * fasta_name) {
  std::ofstream output_file2(fasta_name);
  bool in_island = false;
  int counter = 0;
  for (size_t i = 0; i < sequence.size(); i++) {
    if (statestr[i] == '2') {
      if (!in_island) {
        in_island = true;
        counter ++;
        os << " Island " << counter << ": from " << i << " to ";
        output_file2 << ">Island " << counter << "\n" << sequence[i];
      }
      else {//in island
        output_file2 << sequence[i];
      }
    }
    else {//not island
      if (in_island) {
        in_island = false;
        os << i - 1 << "\n";
        output_file2 << "\n";
      }
    }
  }
  if (in_island) {
    in_island = false;
    os << sequence.size() - 1 << "\n";
  }

  output_file2.close();

}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Please, provide input file name as a first parameter" << std::endl;
    return 0;
  }
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  FastaData fasta;
  fasta.loadFromStream(inputStream);
  inputStream.close();
  std::string str1 = fasta.data[0].second;
  std::string s(str1.c_str());
  std::ofstream output_file1("viterbi_results.txt");
  output_file1 << argv[1] <<std::endl << fasta.data[0].first << std::endl;
  output_file1.precision(4);
  output_file1.setf(std::ios::fixed | std::ios::showpoint);
  for (int i = 0; i < 10; i++) {
    std::cout << "Iteration "<< i <<std::endl;
    output_file1 << "--------------------\nIteration " << i+1 << ": \n";
    get_state_sequence(str1, s);
    update_transitions(s);
    output_file1 << " state1 found " << _state1 << " times, state2 found " << _state2 << " times,\n"
        << " number of segments with A+T state - " << _segment1 << ", number of segments with G+C state - " << _segment2 << std::endl;
    std::cout << "\n " << _state1 << " " << _state2 << " "<<_segment1 << " jj " << _segment2;
    output_file1  << " New transitions probabilities: a11=" << a11 << ", a12=" << a12 << ", a21=" << a21 << ", a22=" << a22 << std::endl;
    a11 = log(a11);
    a12 = log(a12);
    a21 = log(a21);
    a22 = log(a22);
  }
  output_file1 << "-------------------\n------------\nLand of " << _segment2 << " CpG Islands\n";
  print_islands(output_file1, str1, s, "viterbi_islands.fasta");
  //std::cout << s;
  output_file1.close();
  return 0;
}
