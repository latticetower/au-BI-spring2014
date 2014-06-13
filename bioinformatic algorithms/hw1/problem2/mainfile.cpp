#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <locale>
#include "fasta_data.h"

const int GAP_COST = 1;
const int MISMATCH_COST = 1;

enum ArrowDirection{
  Match, 
  Left, 
  Down, 
  Invalid
};
// arrow directions for backtracing

inline int match(char a, char b) {
  return (std::tolower(a) == std::tolower(b) ? 0 : MISMATCH_COST);
}

//updates specified array value based on neighbour values and match/mismatch costs
ArrowDirection set_value(std::vector<int> & holder, int i, int j, int k, std::string const& str_a, std::string const& str_b) {
  ArrowDirection direction = Match;
  if (j + 1 < 0 || j + 1 >= (int)holder.size())
    return Invalid;
  int new_val = holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]);
  if (j < (int)holder.size() && j >= 0) {
    if (holder[j] + GAP_COST < new_val) {
      new_val = holder[j] + GAP_COST;
      direction = Down;
    } 
  }
  if (j + 2 < (int)holder.size() && j + 2 >= 0) {
    if (holder[j + 2] + GAP_COST < new_val) {
      new_val = holder[j + 2] + GAP_COST;
      direction = Left;
    }
  }
  holder[j + 1] = new_val;
  return direction;
}

// resets previously updated value (method reverses cell value modified by set_value)
// method is not used, because it turned out that there is ambiguity in backtracing with 1-dim array
// so, instead of using it array of pointers to directions is used (in main levenshtein dist method)
ArrowDirection reset_value(std::vector<int> & holder, int i, int j, int k, std::string const& str_a, std::string const & str_b) {
  if (j + 1 < 0 || j + 1 >= (int)holder.size())
    return Invalid;

  int preffered_index = j + 1;
  if (j + 2 < 0 || j + 2 >= (int)holder.size()) {
    preffered_index = j;
  }
  else {
    if (j < 0 || j >= (int)holder.size()) {
      preffered_index = j + 2;
    } else {
      preffered_index = (holder[j + 2] < holder[j] ? j + 2 : j);
    }
  }
  ArrowDirection direction = (preffered_index == j ? Down : Left);
  
  
  if (match(str_a[i], str_b[j - k + i + 1]) == 0) {
    if (holder[j + 1] <= holder[preffered_index] + GAP_COST) {
      //holder[j + 1] = holder[j + 1] - match(str_a[i], str_b[j - k + i + 1]);
      direction = Match;
    }
  }
  else {
    if (holder[j + 1] < holder[preffered_index] + GAP_COST && holder[j + 1] >= match(str_a[i], str_b[j - k + i + 1])) {
      holder[j + 1] = holder[j + 1] - match(str_a[i], str_b[j - k + i + 1]);
      direction = Match;
    }
  }
  return direction;
}

struct DirectionHolder{
  ArrowDirection last_direction;
  std::shared_ptr<DirectionHolder> previous;
  DirectionHolder(): last_direction(Invalid) {
    previous = NULL;
  }
  DirectionHolder(ArrowDirection direction, std::shared_ptr<DirectionHolder> prev): last_direction(direction), previous(prev) {
  }
};

std::pair<int, std::vector<ArrowDirection> > levenshtein_dist(std::string & str_a, std::string & str_b, int k) {
  if (str_a.size() < str_b.size())
    return levenshtein_dist(str_b, str_a, k);
  std::vector<int> holder ;
  holder.resize(2*k + 1, 0);
  std::vector<std::shared_ptr<DirectionHolder> > directions;
  directions.resize(2*k + 1);
  //initialization
  holder[k] = 0;
  int min_value = 0, min_position = 0;
  for (int i = 0; i < k; i++) {
    holder[k - i - 1] = holder[k - i] + GAP_COST;
    holder[k + i + 1] = holder[k + i] + GAP_COST;
    
    directions[k + i + 1] = std::shared_ptr<DirectionHolder>(new DirectionHolder(Down, directions[k + i]));
    directions[k - i - 1] = std::shared_ptr<DirectionHolder>(new DirectionHolder(Left, directions[k - i]));
  }

  std::vector<ArrowDirection> backtracing_path;

  for (int i = 0; i < (int)str_a.size(); i++) {
    int j = k - std::min(i + 1, k);
    if (i < k ) {
      min_value = holder[k - i - 1];
      min_position = k - i - 1;
    }
    else {//i>=k
      ArrowDirection direction = set_value(holder, i, -1, k, str_a, str_b);
      if (direction == Match)
        directions[0] = std::shared_ptr<DirectionHolder>(new DirectionHolder(direction, directions[0]));
      if (direction == Left) 
        directions[0] = std::shared_ptr<DirectionHolder>(new DirectionHolder(direction, directions[1]));
      //holder[0] = std::min(holder[0] + match(str_a[i], str_b[i - k]), holder[1] + GAP_COST);
      min_value = holder[0];
      min_position = 0;
    }
  
    for ( ; j < std::min(2*k - 1, (int)str_b.size() + k - i - 1); j ++) {
      ArrowDirection direction = set_value(holder, i, j, k, str_a, str_b);
      if (direction == Match)
        directions[j + 1] = std::shared_ptr<DirectionHolder>(new DirectionHolder(direction, directions[j + 1]));
      if (direction == Left) 
        directions[j + 1] = std::shared_ptr<DirectionHolder>(new DirectionHolder(direction, directions[j + 2]));
      if (direction == Down) 
        directions[j + 1] = std::shared_ptr<DirectionHolder>(new DirectionHolder(direction, directions[j]));
     
      if (holder[j + 1] <= min_value) {
        min_value = holder[j + 1];
        min_position = j + 1;
      }
      
    }
    
    if (min_value >= k)
      return std::make_pair(-1, backtracing_path);
  }
  //  min_position = std::min(2*k, (int)(str_b.size() + k - str_a.size() - 1));
  if (min_value >= k)
      return std::make_pair(-1, backtracing_path);
  //backtracing begins here
  std::shared_ptr<DirectionHolder> last_dir = directions[min_position];
  while (last_dir != NULL) {
    backtracing_path.push_back(last_dir->last_direction);
    last_dir = last_dir->previous;
  }
  //backtracing ends here

  return std::make_pair(min_value, backtracing_path);  
}

std::string get_result_string(std::string const& str, std::vector<ArrowDirection> & path, int n) {
  std::string buffer = "";
  int i = 0;
  for (std::vector<ArrowDirection>::reverse_iterator iter = path.rbegin(); iter != path.rend(); ++iter) {
    
    if (*iter == Match) {
      if (i >= (int)str.size())
        break;
      buffer.push_back(str[i]); 
      i++;
      continue;
    }
    if (*iter == Left) {
      if (n == 1) {
        if (i >= (int)str.size())
          break;
        buffer.push_back(str[i]); i++;
      }
      else {
        buffer.push_back('-');
      }
    }
    if (*iter == Down) {
      if (n == 1) {
        buffer.push_back('-');
      }
      else {
        if (i >= (int)str.size())
          break;
        buffer.push_back(str[i]); i++;
      }
    }
  }
  return buffer;
}

std::string get_result_string2(std::string const & str_a, std::string const & str_b, std::vector<ArrowDirection> & path, int n) {
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
  
  std::ofstream output_file(argv[2]);
  
  std::pair<int, std::vector<ArrowDirection> > result = levenshtein_dist(fasta.data[0].second, fasta.data[1].second, 100);
  if (result.first >= 0)
    output_file //<< result.first << std::endl 
    << get_result_string2(fasta.data[0].second, fasta.data[1].second, result.second, 1) << std::endl 
    << get_result_string2(fasta.data[0].second, fasta.data[1].second, result.second, 2);
  else output_file<< "not similar";
  output_file.close();
  return 0;
}