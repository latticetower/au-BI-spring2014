#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <set>
#include <map>

struct Graph{
  unsigned int n;//vertices count;
  std::map<unsigned int, std::set<std::pair<unsigned int, unsigned long long> > > adj_list;
  std::vector<long double> subtree_weights;
  std::vector<unsigned long long> subtree_paths;
  std::vector<long double> subtree_weights_ends_at_current;

  Graph() {}

  void loadAdjList(std::istream & input_stream) {
    input_stream >> n;
    while (!input_stream.eof()) {
      int a, b;
      unsigned long long weight;
      input_stream >> a >> b >> weight;
      if (adj_list.find(a) == adj_list.end()) {
        adj_list[a] = std::set<std::pair<unsigned int, unsigned long long> >();
      }
      if (adj_list.find(b) == adj_list.end()) {
        adj_list[b] = std::set<std::pair<unsigned int, unsigned long long> >();
      }
      adj_list[a].insert(std::make_pair(b, weight));
      adj_list[b].insert(std::make_pair(a, weight));
    }

    subtree_weights.resize(n + 1,0.0);
    subtree_paths.resize(n + 1, 0);
    subtree_weights_ends_at_current.resize(n + 1, 0.0);
  }

  //
  void find_max_subtree(unsigned int parent, unsigned int current, long double d) {
    if (parent > 0 && adj_list[current].size() <= 1) { //that means that current vertex is a leaf
      //values[current].first = 0;
      //values[current].second = 1;
    }
    else {
      for (std::set<std::pair<unsigned int, unsigned long long> >::iterator iter = adj_list[current].begin(); iter != adj_list[current].end(); ++ iter) {
        if (iter->first == parent)
          continue;
        find_max_subtree(current, iter->first, d);
        subtree_paths[current] += subtree_paths[iter->first] + 1;
        subtree_weights_ends_at_current[current] += subtree_weights_ends_at_current[iter->first] + (subtree_paths[iter->first]*iter->second + iter->second)/d;
        subtree_weights[current] += subtree_weights[iter->first];
       
      }
     
      for (std::set<std::pair<unsigned int, unsigned long long> >::iterator iter = adj_list[current].begin(); iter != adj_list[current].end(); ++ iter) {
        if (iter->first == parent)
          continue;
        subtree_weights[current] += (subtree_weights_ends_at_current[iter->first] + (subtree_paths[iter->first]*iter->second + iter->second)/d)*
                            (subtree_paths[current] - subtree_paths[iter->first] -1);
        
      }
      subtree_weights[current] += subtree_weights_ends_at_current[current];
    }
  }

  long double computeWeights() {
    if (adj_list.size() == 0)
      return 0;
    unsigned int startvertex = (adj_list.begin())->first;
    long double d = n*(n-1)/2.0;
    find_max_subtree(0, startvertex, d);

    return subtree_weights[startvertex];
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

  
  std::ofstream output_file(argv[2]);
  output_file << graph.computeWeights();
  
  output_file.close();
  return 0;
}