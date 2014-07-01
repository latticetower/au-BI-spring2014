#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <locale>
#include "fasta_data.h"
#include "distance_tools.h"

std::string buffer = "";

std::string const & get_result_string(std::string const & str,
                                      BacktracingPath const & path,
                                      int n) {
  size_t i = 0;
  for (BacktracingPath::const_reverse_iterator iter = path.rbegin(); iter != path.rend(); ++iter) {
      if (iter->direction == Match) {
          for (size_t j = 0; j < iter->length; j++) {
              if (i >= str.size())
                  break;
              buffer.push_back(str[i]);
              i++;
          }
          continue;
      }
      if (iter->direction == Left) {
          if (n == 1) {
              for (size_t j = 0; j < iter->length; j++) {
                  if (i >= str.size())
                      break;
                  buffer.push_back(str[i]);
                  i++;
              }
          }
          else {
              for (size_t j = 0; j < iter->length; j++) {
                  buffer.push_back('-');
              }
          }
      }
      if (iter->direction == Down) {
          if (n == 1) {
              for (size_t j = 0; j < iter->length; j++) {
                  buffer.push_back('-');
              }
          }
          else {
              for (size_t j = 0; j < iter->length; j++) {
                    if (i >= str.size())
                        break;
                    buffer.push_back(str[i]);
                    i++;
                }
            }
        }
    }
    return buffer;
}

std::string const & get_result_string2(std::string const& str_a,
                               std::string const& str_b,
                               BacktracingPath const& path,
                               int n) {
  if (str_a.size() < str_b.size())
    return get_result_string2(str_b, str_a, path, n);
  return get_result_string((n == 1 ? str_a : str_b), path, n);
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

    size_t k = 100;

    std::ofstream output_file(argv[2]);
    DistanceEstimator estimator(k);
    long long start_time = time(NULL);

    LevenshteinInfo const & result = estimator.levenshtein_dist_info(fasta.data[0].second, fasta.data[1].second);
    long long end_time = time(NULL);
    std::cout << "time elapsed (without backtracing, minutes) " << (end_time - start_time)*1.0/60 << std::endl;

    if (result.distance < k)
        output_file //<< result.first << std::endl
        << get_result_string2(fasta.data[0].second, fasta.data[1].second, result.backtracing_path, 1) << std::endl
        << get_result_string2(fasta.data[0].second, fasta.data[1].second, result.backtracing_path, 2);
    else output_file << "not similar";
    output_file.close();

    end_time = time(NULL);
    std::cout << "Total time elapsed (minutes): " << (end_time - start_time)*1.0/60 << std::endl;
    return 0;
}
