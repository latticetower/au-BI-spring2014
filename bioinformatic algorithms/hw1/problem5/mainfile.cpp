#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <locale>
#include "fasta_data.h"

const int GAP_COST = 1;
const int MISMATCH_COST = 1;
const int MATCH_COST = 0;


inline int match(char a, char b) {
  return (std::tolower(a) == std::tolower(b) ? MATCH_COST : MISMATCH_COST);
}

inline int select_better_score(int a, int b) {
  return std::min(a, b);
  // change to max for using with negative gap and indel cost and positive match cost
}


void print_matrix(std::vector<std::vector<int> > & holder) {
  for (size_t i = 0; i < holder.size(); i++) {
    for (size_t j = 0; j < holder[i].size () ; j++){
      std::cout << holder [i][j] << " ";
    }
    std::cout << std::endl;
  }
}
std::pair<std::string, std::string> get_aligned_string_a(std::string const& str_a, std::string const& str_b,
                std::vector<std::vector<int> > const & holder, int result) {
  int n = str_a.size();
  int m = str_b.size();

  //first find last str_a substring index:
  int last_position_i = m - 1;
  int last_position_j = 0;
  for (int j = 0; j < n; j++) {
    if (holder[m][j + 1] == result)
      last_position_j = j;
  }

  //maybe should add here some error handling?
  std::string str_a_aligned = "";
  std::string str_b_aligned = "";
  while (last_position_i >= 0) {
    int match_res = holder[last_position_i][last_position_j] +
          match(str_b[last_position_i], str_a[last_position_j]);
    int mismatch_i = holder[last_position_i + 1][last_position_j] + GAP_COST;
    //int mismatch_j = holder[last_position_i][last_position_j + 1] + GAP_COST;
    int min_score = holder[last_position_i + 1][last_position_j + 1];
    if (min_score == match_res) {
      str_a_aligned.push_back(str_a[last_position_j--]);
      str_b_aligned.push_back(str_b[last_position_i--]);

      continue;
    }
    if (min_score == mismatch_i) {
      str_a_aligned.push_back(str_a[last_position_j--]);
      str_b_aligned.push_back('-');
    } else {
      str_a_aligned.push_back('-');
      str_b_aligned.push_back(str_b[last_position_i--]);
    }

  }

  //
  std::reverse(str_a_aligned.begin(), str_a_aligned.end());
  std::cout << str_a_aligned;
  std::reverse(str_b_aligned.begin(), str_b_aligned.end());
  return std::make_pair(str_a_aligned, str_b_aligned);

}

std::pair<int, std::pair<std::string, std::string> > levenshtein_dist2(std::string const& str_a, std::string const & str_b, bool debug = false) {
  std::vector<std::vector<int> > holder;
  int n = str_a.size();
  int m = str_b.size();
  holder.resize(m + 1);
  holder[0].resize(n + 1, 0);
  for (int j = 1; j <= n; j++) {
    holder[0][j] = 0;
  }
  for (int i = 1; i <= m; i++) {
    holder[i].resize(n + 1, 0);
    holder[i][0] = holder[i - 1][0] + GAP_COST;
    // initalization;
    // holder[0][j] == 0 for all j
  }
  if (debug)
    print_matrix(holder);
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      int min_score = select_better_score(
        select_better_score(holder[i][j] + match(str_b[i], str_a[j]),
                            holder[i + 1][j] + GAP_COST),
        holder[i][j + 1] + GAP_COST);
      holder[i + 1][j + 1] = min_score;
    }
  }
  int result = holder[m][0];
  for (int j = 1; j <= n; j++) {
    result = select_better_score(result, holder[m][j]);
  }

  if (debug)
    print_matrix(holder);
  std::pair<std::string, std::string> s = get_aligned_string_a(str_a, str_b, holder, result);
  return std::make_pair(result, s);
}


int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "Please, provide input file name as a first parameter, and output file name as a second parameter" << std::endl;
    return 0;
  }
  FastaData fasta;
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  fasta.loadFromStream(inputStream);
  inputStream.close();

  std::ofstream output_file(argv[2]);

  std::pair<int, std::pair<std::string, std::string> > result = levenshtein_dist2(fasta.data[0].second, fasta.data[1].second);
  output_file << result.first << std::endl
  << result.second.first << std::endl << result.second.second;
  output_file.close();
  return 0;
}
