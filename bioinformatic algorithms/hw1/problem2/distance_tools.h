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
    DirectionHolder(ArrowDirection const & direction,
                    std::shared_ptr<DirectionHolder> prev): last_direction(direction), previous(prev), count(1), prev_count(0) {
        if (prev != NULL) {
          prev_count = prev->count;
        }
    }
};

struct PathStep{
    ArrowDirection direction;
    size_t length;
    PathStep(ArrowDirection const & d, size_t const & l) : direction(d), length(l) { }
    PathStep(std::pair<ArrowDirection, size_t> const & p): direction(p.first), length(p.second) {}
};

typedef std::vector<PathStep> BacktracingPath;

struct LevenshteinInfo{
    size_t distance;
    BacktracingPath backtracing_path;
    LevenshteinInfo(): distance(-1) {}
    void push_back(std::pair<ArrowDirection, size_t> & p) {
        backtracing_path.push_back(PathStep(p));
    }

    void push_back(ArrowDirection const & d, size_t const & p) {
        backtracing_path.push_back(PathStep(d, p));
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
    inline size_t match(char const & a, char const & b) {
        //return (std::tolower(a) == std::tolower(b) ? 0 : MISMATCH_COST);
        return (a == b ? 0 : MISMATCH_COST);
    }

    DistanceEstimator(size_t k_): k(k_) {  }

    ~DistanceEstimator() {  }

  private:
    inline void update_directions(size_t const & index,
                                  ArrowDirection const & direction
                                  ) {
        if (direction == Invalid)
            return;
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
        init_vectors(/*directions, holder*/);
        info.clear();
        //long long start_time = time(NULL);
        //long long end_time = 0;
        size_t i = 0;
        size_t j_min, j_max;
        for (i = 0; i < str_a.size(); i++) {
            /*if (i% 200000==0) {
              end_time = time(NULL);
              std::cout << "time on " << i << ": " << end_time - start_time << std::endl;
            }*/
            j_min = std::max(k - i - 1, min_value);
            j_max = std::min(2*k - 1 - min_value, str_b.size() + k - i - 1);

            if (i >= k) {
                j_min = min_value;
                set_value_and_update_direction(/*holder,*/ i, j_min - 1, k, str_a, str_b, j_min, j_max);
            }
            min_position = j_min;
            min_value = holder[min_position];
            for (size_t j = j_min; j < j_max; j ++) {
                set_value_and_update_direction(/*holder,*/ i, j, k, str_a, str_b, j_min, j_max);
                if (holder[j + 1] <= min_value){
                    min_position = j + 1;
                    min_value = holder[min_position];
                }
            }
            if (min_value >= k || j_max <= j_min)
                return info;
        }

        if (min_value >= k)
            return info;
        return load_backtracing_path(/*info,*/ min_position/*, directions*/);
    }

  protected:
    inline void init_vectors(
            /*std::vector<std::shared_ptr<DirectionHolder> > & directions,
            std::vector<size_t> & holder*/
            ) {

        for (size_t i = 0, kprev = k - 1, knext = k + 1; i < k; i++, kprev --, knext ++) {
            holder[kprev] = holder[kprev + 1] + GAP_COST;
            holder[knext] = holder[knext - 1] + GAP_COST;

            update_directions(knext, Down);
            update_directions(kprev, Left);
        }
    }


    inline size_t get_index(size_t const & current_index, ArrowDirection const & direction) {
        if (direction == ArrowDirection::Match)
            return current_index;
        if (direction == ArrowDirection::Left)
            return current_index + 1;
        return current_index - 1;
    }

    LevenshteinInfo const &  load_backtracing_path(
            /*LevenshteinInfo & info,*/
            size_t & min_position/*,
            std::vector<std::shared_ptr<DirectionHolder> > & directions*/
            ) {
        std::shared_ptr<DirectionHolder> last_dir = directions[min_position];
        size_t prev_count = -1;
        while (last_dir != NULL) {
            if (!info.backtracing_path.empty()) {
                PathStep & p = info.backtracing_path.back();
                if (p.direction == last_dir->last_direction) {
                    p.length += std::min(last_dir->count, prev_count);
                } else {
                  info.push_back(last_dir->last_direction, std::min(last_dir->count, prev_count));
                }
            } else {
                info.push_back(last_dir->last_direction, std::min(last_dir->count, prev_count));
            }

            prev_count = last_dir->prev_count;
            last_dir = last_dir->previous;
        }
        info.distance = holder[min_position];
        return info;
    }

    //updates specified array value based on neighbour values and match/mismatch costs
    inline ArrowDirection set_value(
              /*std::vector<size_t> & holder,*/
              size_t const & i, size_t const & j, size_t const & k,
              std::string const & str_a,
              std::string const & str_b,
              size_t const & j_min,
              size_t const & j_max
              ) {
        ArrowDirection direction = Match;
        if (j + 1 >= j_max || j + 1 < j_min)
            return Invalid;
        size_t new_val = holder[j + 1] + match(str_a[i], str_b[j - k + i + 1]);
        if (j >= j_min && j < j_max && holder[j] + GAP_COST < new_val) {
            new_val = holder[j] + GAP_COST;
            direction = Down;
        }
        if (j + 2 >= j_min && j + 2 < j_max && holder[j + 2] + GAP_COST < new_val) {
            new_val = holder[j + 2] + GAP_COST;
            direction = Left;
        }
        holder[j + 1] = new_val;
        return direction;
    }

    inline void set_value_and_update_direction(
            /*std::vector<size_t> & holder,*/
            size_t const & i, size_t const & j, size_t const & k,
            std::string const & str_a,
            std::string const & str_b,
            size_t const & j_min,
            size_t const & j_max
            ) {
        ArrowDirection direction = set_value(/*holder,*/ i, j, k, str_a, str_b, j_min, j_max);
        update_directions(j + 1, direction);
    }

  private:
    size_t k;
    std::vector<size_t> holder;
    std::vector<std::shared_ptr<DirectionHolder> > directions;
    LevenshteinInfo info;
};
