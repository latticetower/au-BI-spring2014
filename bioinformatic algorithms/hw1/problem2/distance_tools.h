#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <locale>

enum ArrowDirection{ Match, Left, Down, Invalid };

class DistanceEstimator{
  public:
    static const int GAP_COST = 1;
    static const int MISMATCH_COST = 1;

    // arrow directions for backtracing
    inline int match(char a, char b) {
      return (std::tolower(a) == std::tolower(b) ? 0 : MISMATCH_COST);
    }

    DistanceEstimator(int k_): k(k_) {
    }

    ~DistanceEstimator() {
      //for (size_t i = 0; i < directions_list.size(); i++) {
      //  delete directions_list[i];
      //}
      //directions_list.clear();
    }
  private:
    void update_directions(int index, ArrowDirection direction, int link_to) {
      if (directions[link_to] != NULL && directions[link_to]->last_direction == direction && index == link_to) {
        directions[link_to]->count++;
        return;
      }
      directions[index] = std::shared_ptr<DirectionHolder>(new DirectionHolder(direction, directions[link_to]));
    }
  public:
    std::pair<int, std::vector<std::pair<ArrowDirection, int> > > levenshtein_dist(std::string & str_a, std::string & str_b) {
      if (str_a.size() < str_b.size())
        return levenshtein_dist(str_b, str_a);
      std::vector<int> holder ;
      holder.resize(2*k + 1, 0);
      directions.clear();
      directions.resize(2*k + 1);
      //initialization
      holder[k] = 0;
      int min_value = 0, min_position = 0;
      for (int i = 0; i < k; i++) {
        holder[k - i - 1] = holder[k - i] + GAP_COST;
        holder[k + i + 1] = holder[k + i] + GAP_COST;
    
        update_directions(k + i + 1, Down, k + i);
        update_directions(k - i - 1, Left, k - i);
      }

      std::vector<std::pair<ArrowDirection, int> > backtracing_path;

      for (int i = 0; i < (int)str_a.size(); i++) {
        int j = k - std::min(i + 1, k);
        if (i < k ) {
          min_value = holder[k - i - 1];
          min_position = k - i - 1;
        }
        else {//i>=k
          ArrowDirection direction = set_value(holder, i, -1, k, str_a, str_b);
          if (direction == Match)
            update_directions(0, direction, 0);
          if (direction == Left) 
            update_directions(0, direction, 1);
          //holder[0] = std::min(holder[0] + match(str_a[i], str_b[i - k]), holder[1] + GAP_COST);
          min_value = holder[0];
          min_position = 0;
        }
  
        for ( ; j < std::min(2*k - 1, (int)str_b.size() + k - i - 1); j ++) {
          ArrowDirection direction = set_value(holder, i, j, k, str_a, str_b);
          if (direction == Match)
            update_directions(j + 1, direction, j + 1);
          if (direction == Left) 
            update_directions(j + 1, direction, j + 2);
          if (direction == Down) 
            update_directions(j + 1, direction, j);
     
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
      size_t prev_count = -1;
      while (last_dir != NULL) {
        if (backtracing_path.size() > 0 && backtracing_path.back().first == last_dir->last_direction) {
          backtracing_path.back().second += std::min(last_dir->count, prev_count);
        } else {
          backtracing_path.push_back(std::make_pair(last_dir->last_direction, std::min(last_dir->count, prev_count)));
        }
        prev_count = last_dir->prev_count;
        last_dir = last_dir->previous;
      }
      //backtracing ends here

      return std::make_pair(min_value, backtracing_path);  
    }

  protected:  
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

  private:
    int k;
    std::vector<int> holder;
    std::vector<std::shared_ptr<DirectionHolder> > directions;
    //std::vector<DirectionHolder> directions_list;// helper construction to delete all pointers to directions without segmentation faults
    std::vector<ArrowDirection> backtracing_path;

};