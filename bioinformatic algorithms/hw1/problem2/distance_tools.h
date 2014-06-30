#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <locale>

enum ArrowDirection{ Match, Left, Down, Invalid };

struct DirectionHolder{
  ArrowDirection last_direction;
  std::shared_ptr<DirectionHolder> previous;
  size_t count, prev_count;
  DirectionHolder(): last_direction(Invalid) {
    previous = NULL;
    count = 0;
  }
  DirectionHolder(ArrowDirection direction, std::shared_ptr<DirectionHolder> prev): last_direction(direction), previous(prev), count(1), prev_count(0) {
    if (prev != NULL) {
      prev_count = prev->count;
    }
  }
};

struct PathStep{
  ArrowDirection direction;
  int length;
  PathStep(ArrowDirection d, int l) : direction(d), length(l) { }
  PathStep(std::pair<ArrowDirection, int> & p): direction(p.first), length(p.second) {}
};

typedef std::vector<PathStep> BacktracingPath;

struct LevenshteinInfo{
  int distance;
  BacktracingPath backtracing_path;
  LevenshteinInfo(): distance(-1) {}
  void push_back(std::pair<ArrowDirection, int> p) {
    backtracing_path.push_back(PathStep(p));
  }
};

class DistanceEstimator{
  public:

    static const int GAP_COST = 1;
    static const int MISMATCH_COST = 1;

    // arrow directions for backtracing
    inline int match(char a, char b) {
      return (std::tolower(a) == std::tolower(b) ? 0 : MISMATCH_COST);
    }

    DistanceEstimator(int k_): k(k_) {  }

    ~DistanceEstimator() {  }

  private:
    void update_directions(int index, ArrowDirection direction) {
      int link_to = get_index(index, direction);
      if (directions[link_to] != NULL && directions[link_to]->last_direction == direction && direction == Match) {
        directions[link_to]->count++;
        return;
      }
      directions[index] = std::shared_ptr<DirectionHolder>(new DirectionHolder(direction, directions[link_to]));
    }
  public:
    LevenshteinInfo levenshtein_dist_info(std::string const& str_a, std::string const& str_b) {
      if (str_a.size() < str_b.size())
        return levenshtein_dist_info(str_b, str_a);

      holder.resize(2*k + 1, 0);
      directions.resize(2*k + 1, NULL);
      //initialization
      int min_position = 0;
      init_vectors(directions, holder);

      BacktracingPath backtracing_path;

      for (int i = 0; i < (int)str_a.size(); i++) {
        int j = k - std::min(i + 1, k);
        if (i < k ) {
          min_position = k - i - 1;
        }
        else {//i>=k
          set_value_and_update_direction(holder, i, -1, k, str_a, str_b);
          min_position = 0;
        }
  
        for ( ; j < std::min(2*k - 1, (int)str_b.size() + k - i - 1); j ++) {
          set_value_and_update_direction(holder, i, j, k, str_a, str_b);
          
          if (holder[j + 1] <= holder[min_position]) 
            min_position = j + 1;
        }
    
        if (holder[min_position] >= k)
          return LevenshteinInfo();
      }
      
      if (holder[min_position] >= k)
          return LevenshteinInfo();
      //do some actual backtracing and return:
      LevenshteinInfo info;
      load_backtracing_path(info, min_position, directions);
      
      return info;  
    }

  protected:  
    void init_vectors(std::vector<std::shared_ptr<DirectionHolder> > & directions, std::vector<int> & holder) {  
      for (int i = 0; i < k; i++) {
        holder[k - i - 1] = holder[k - i] + GAP_COST;
        holder[k + i + 1] = holder[k + i] + GAP_COST;
    
        update_directions(k + i + 1, Down);
        update_directions(k - i - 1, Left);
      }
    }

    size_t get_index(size_t current_index, ArrowDirection direction) {
      if (direction == ArrowDirection::Match) 
        return current_index;
      if (direction == ArrowDirection::Left)
        return current_index + 1;
      return current_index - 1;
    }

    void load_backtracing_path(LevenshteinInfo& info, int min_position, std::vector<std::shared_ptr<DirectionHolder> > & directions) {
      std::shared_ptr<DirectionHolder> last_dir = directions[min_position];
      size_t prev_count = -1;
      while (last_dir != NULL) {
        if (info.backtracing_path.size() > 0 && info.backtracing_path.back().direction == last_dir->last_direction) {
          info.backtracing_path.back().length += std::min(last_dir->count, prev_count);
        } else {
          info.push_back(std::make_pair(last_dir->last_direction, std::min(last_dir->count, prev_count)));
        }

        prev_count = last_dir->prev_count;
        last_dir = last_dir->previous;
      }
      info.distance = holder[min_position];
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

    void set_value_and_update_direction(std::vector<int> & holder, int i, int j, int k, std::string const& str_a, std::string const& str_b) {
      ArrowDirection direction = set_value(holder, i, j, k, str_a, str_b);
      update_directions(j + 1, direction);
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

  private:
    int k;
    std::vector<int> holder;
    std::vector<std::shared_ptr<DirectionHolder> > directions;
    //std::vector<DirectionHolder> directions_list;// helper construction to delete all pointers to directions without segmentation faults
    
};