#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>


struct Graph{
  int n;//vertices count;
  std::map<int, std::set<int> > adj_list;
  std::vector<std::pair<int, int> > values;//use this array to store max values for given vertex

  Graph() {}

  void loadAdjList(std::istream & input_stream) {
    input_stream >> n;
    values.resize(n + 1, std::pair<int, int> (0, 1));

    while (!input_stream.eof()) {
      int a, b;
      input_stream >> a >> b;
      if (adj_list.find(a) == adj_list.end()) {
        adj_list[a] = std::set<int>();
      }
      if (adj_list.find(b) == adj_list.end()) {
        adj_list[b] = std::set<int>();
      }
      adj_list[a].insert(b);
      adj_list[b].insert(a);
    }
  }
  void recursive_max_set_size(int parent, int current) {
    if (parent > 0 && adj_list[current].size() <= 1) { //that means that current vertex is a leaf
      //values[current].first = 0;
      //values[current].second = 1;
    }
    else {
      //values[current].second = 1;
      for (std::set<int>::iterator iter = adj_list[current].begin(); iter != adj_list[current].end(); ++ iter) {
        if (*iter == parent)
          continue;
        recursive_max_set_size(current, *iter);
        values[current].first  += std::max(values[*iter].first, values[*iter].second);
        values[current].second += values[*iter].first;
      }
    }
  }

  std::set<int> recursive_max_set(int parent, int current, int current_max_value) {
    std::set<int> result;
    if (current_max_value == values[current].second) { //this means we include current vertex to result set
      result.insert(current);
      for (std::set<int>::iterator iter = adj_list[current].begin(); iter != adj_list[current].end(); ++ iter) {
        if (*iter == parent)
          continue;
        std::set<int> res = recursive_max_set(current, *iter, values[*iter].first);
        result.insert(res.begin(), res.end());
      }
    }
    else {//this means we don't include current vertex
      for (std::set<int>::iterator iter = adj_list[current].begin(); iter != adj_list[current].end(); ++ iter) {
        if (*iter == parent)
          continue;
        if (values[*iter].first > values[*iter].second) {
          std::set<int> res = recursive_max_set(current, *iter, values[*iter].first);
          result.insert(res.begin(), res.end());
        }
        else {
          std::set<int> res = recursive_max_set(current, *iter, values[*iter].second);
          result.insert(res.begin(), res.end());
        }
      }
    }
    return result;
  }
  std::pair<int, std::set<int> > get2Coloring() {
    std::pair<int, std::set<int> > result;
    if (adj_list.size() == 0)
      return result;
    int startvertex = (adj_list.begin())->first;
    recursive_max_set_size(0, startvertex);
    result.first = std::max(values[startvertex].first, values[startvertex].second);
    result.second = recursive_max_set(0, startvertex, result.first);
    return result;
  }
};

int main(int argc, char** argv){
  //let's assume that 
  // 1. all vertices are connected
  // 2. adjacency list describes tree
  // 3. graph is undirected
  Graph graph;
  if (argc < 3) {
    std::cout << "Please, provide input file name as a first parameter, and output file name as a second parameter" << std::endl;
    return 0;
  }
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  graph.loadAdjList(inputStream);
  inputStream.close();
  std::pair<int, std::set<int> > coloring = graph.get2Coloring();
  
  std::ofstream output_file(argv[2]);
 
  output_file << coloring.first << std::endl;
  for (std::set<int>::iterator iter = coloring.second.begin(); iter != coloring.second.end(); ++iter) {
    output_file << *iter << " ";
  }

  
  output_file.close();
  return 0;
}