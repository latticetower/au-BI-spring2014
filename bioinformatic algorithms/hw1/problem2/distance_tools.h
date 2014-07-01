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
    size_t length;
    PathStep(ArrowDirection d, size_t l) : direction(d), length(l) { }
    PathStep(std::pair<ArrowDirection, size_t> & p): direction(p.first), length(p.second) {}
};

typedef std::vector<PathStep> BacktracingPath;

struct LevenshteinInfo{
    size_t distance;
    BacktracingPath backtracing_path;
    LevenshteinInfo(): distance(-1) {}
    void push_back(std::pair<ArrowDirection, size_t> p) {
        backtracing_path.push_back(PathStep(p));
    }
    void clear() {
        distance = -1;
        backtracing_path.clear();
    }
};


class DistanceEstimator{
  public:

    static const size_t GAP_COST = 1;
    static const size_t MISMATCH_COST = 1;

    // arrow directions for backtracing
    inline size_t match(char a, char b) {
      return (std::tolower(a) == std::tolower(b) ? 0 : MISMATCH_COST);
    }

    DistanceEstimator(size_t k_): k(k_) {  }

    ~DistanceEstimator() {  }

  private:
    inline void update_directions(size_t index, ArrowDirection direction) {
        size_t link_to = get_index(index, direction);
        if (directions[link_to] != NULL &&
            directions[link_to]->last_direction == direction &&
            direction == Match
            ) {
            directions[link_to]->count++;
            return;
        }
        directions[index] = std::shared_ptr<DirectionHolder>(
                              new DirectionHolder(
                                        direction,
                                        directions[link_to]
                                        )
                            );
    }

  public:

    LevenshteinInfo const & levenshtein_dist_info(
            std::string const & str_a,
            std::string const & str_b
            ) {
        //std::cout << "start operation: " << time(NULL) << std::endl;
        if (str_a.size() < str_b.size())
            return levenshtein_dist_info(str_b, str_a);

        holder.resize(2*k + 1, 0);
        directions.resize(2*k + 1, NULL);
        //initialization
        size_t min_position = 0, min_value = 0;
        init_vectors(directions, holder);
        info.clear();
        //long long start_time = time(NULL);
        //long long end_time = 0;
        size_t i = 0;
        size_t j_min, j_max;
        for (i = 0; i < str_a.size(); i++) {

            j_min = std::max(k - i - 1, min_value);
            j_max = std::min(2*k - 1 - min_value, str_b.size() + k - i - 1);

            if (i >= k) {
                j_min = min_value;
                set_value_and_update_direction(holder, i, -1, k, str_a, str_b);
            }
            min_position = j_min;
            min_value = holder[min_position];
            if (j_max >= j_min)
                return info;
            for (size_t j = j_min; j < j_max; j ++) {
                set_value_and_update_direction(holder, i, j, k, str_a, str_b);
                if (holder[j + 1] <= holder[min_position]){
                    min_position = j + 1;
                    min_value = holder[min_position];
                }
            }
            if (min_value >= k)
                return info;
        }

        if (min_value >= k)
            return info;
        return load_backtracing_path(info, min_position, directions);
    }

  protected:
    inline void init_vectors(
            std::vector<std::shared_ptr<DirectionHolder> > & directions,
            std::vector<size_t> & holder
            ) {
        for (size_t i = 0; i < k; i++) {
            holder[k - i - 1] = holder[k - i] + GAP_COST;
            holder[k + i + 1] = holder[k + i] + GAP_COST;

            update_directions(k + i + 1, Down);
            update_directions(k - i - 1, Left);
        }
    }


    inline size_t get_index(size_t current_index, ArrowDirection direction) {
        if (direction == ArrowDirection::Match)
            return current_index;
        if (direction == ArrowDirection::Left)
            return current_index + 1;
        return current_index - 1;
    }

    LevenshteinInfo const &  load_backtracing_path(
            LevenshteinInfo & info,
            size_t min_position,
            std::vector<std::shared_ptr<DirectionHolder> > & directions
            ) {
        std::shared_ptr<DirectionHolder> last_dir = directions[min_position];
        size_t prev_count = -1;
        while (last_dir != NULL) {
            if (info.backtracing_path.size() > 0 &&
                info.backtracing_path.back().direction == last_dir->last_direction
                ) {
                info.backtracing_path.back().length += std::min(last_dir->count, prev_count);
            } else {
                info.push_back(std::make_pair(last_dir->last_direction, std::min(last_dir->count, prev_count)));
            }

            prev_count = last_dir->prev_count;
            last_dir = last_dir->previous;
        }
        info.distance = holder[min_position];
        return info;
    }

    //updates specified array value based on neighbour values and match/mismatch costs
    inline ArrowDirection set_value(
              std::vector<size_t> & holder,
              size_t i, size_t j, size_t k,
              std::string const & str_a,
              std::string const & str_b
              ) {
        ArrowDirection direction = Match;
        if (j + 1 >= holder.size())
            return Invalid;
        size_t new_val = holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]);
        if (j < holder.size() && holder[j] + GAP_COST < new_val) {
            new_val = holder[j] + GAP_COST;
            direction = Down;
        }
        if (j + 2 < holder.size() && holder[j + 2] + GAP_COST < new_val) {
            new_val = holder[j + 2] + GAP_COST;
            direction = Left;
        }
        holder[j + 1] = new_val;
        return direction;
    }

    inline void set_value_and_update_direction(
            std::vector<size_t> & holder,
            size_t i, size_t j, size_t k,
            std::string const & str_a,
            std::string const & str_b
            ) {
        ArrowDirection direction = set_value(holder, i, j, k, str_a, str_b);
        update_directions(j + 1, direction);
    }

    // resets previously updated value (method reverses cell value modified by set_value)
    // method is not used, because it turned out that there is ambiguity in backtracing with 1-dim array
    // so, instead of using it array of pointers to directions is used (in main levenshtein dist method)
    ArrowDirection reset_value(
            std::vector<size_t> & holder,
            size_t i, size_t j, size_t k,
            std::string const & str_a,
            std::string const & str_b
            ) {
        if (j + 1 >= holder.size())
            return Invalid;

        size_t preffered_index = j + 1;
        if (j + 2 >= holder.size()) {
            preffered_index = j;
        }
        else {
            if (j >= holder.size()) {
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
    size_t k;
    std::vector<size_t> holder;
    std::vector<std::shared_ptr<DirectionHolder> > directions;
    LevenshteinInfo info;
};
