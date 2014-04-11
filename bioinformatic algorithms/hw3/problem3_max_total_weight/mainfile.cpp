#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include<stack>
#include <map>


struct Graph{
  int n, q;//vertices count;
  std::map<int, std::set<std::pair<int, long double> > > adj_list;
  std::vector<std::vector<long double> > subweights;
  std::vector<int> maxedges;
  Graph() {}

  void loadAdjList(std::istream & input_stream) {
    input_stream >> n >> q;
    while (!input_stream.eof()) {
      int a, b;
      long double weight;
      input_stream >> a >> b >> weight;
      adj_list[a].insert(std::make_pair(b, weight));
      adj_list[b].insert(std::make_pair(a, weight));
     
    }
    subweights.resize(n + 1);
    for (std::vector<std::vector<long double> >::iterator iter = subweights.begin(); iter != subweights.end(); ++iter ) {
      iter->resize(q + 1, 0.0);
    }
    maxedges.resize(n + 1, 0);
  }

  void find_max_edges(int parent, int current) {
    if (parent > 0 && adj_list[current].size() <= 1) { //that means that current vertex is a leaf
      //values[current].first = 0;
      //values[current].second = 1;
    }
    else {
      //values[current].second = 1;
      for (std::set<std::pair<int, long double> >::iterator iter = adj_list[current].begin(); iter != adj_list[current].end(); ++ iter) {
        if (iter->first == parent)
          continue;
        find_max_edges(current, iter->first);
        maxedges[current] += maxedges[iter->first] + 1;
      }

    }
  }

    void find_max_subtree(int parent, int current) {
    if (parent > 0 && adj_list[current].size() <= 1) { //that means that current vertex is a leaf
      //values[current].first = 0;
      //values[current].second = 1;
    }
    else {
      //values[current].second = 1;
      for (std::set<std::pair<int, long double> >::iterator iter = adj_list[current].begin(); iter != adj_list[current].end(); ++ iter) {
        if (iter->first == parent)
          continue;
        find_max_subtree(current, iter->first);
        
        for (int i = std::min(q, maxedges[current]); i >= 1; i--) {
          for (int j = 0; j < i ; j++) {
            if (i - j - 1 <= maxedges[iter->first])
            subweights[current][i] = std::max(subweights[current][i], subweights[current][j] + subweights[iter->first][i-j-1] + iter->second);
          }
        }
        //subweights[current].first  += std::max(subweights[*iter].first, subweights[*iter].second);
        //subweights[current].second += subweights[*iter].first;
      }
    }
  }

  long double computeWeights() {
    if (adj_list.size() == 0)
      return 0;
    int startvertex = (adj_list.begin())->first;
    find_max_edges(0, startvertex);
    find_max_subtree(0, startvertex);
    long double result = subweights[startvertex][0];
    for (std::vector<long double>::iterator iter = subweights[startvertex].begin(); iter != subweights[startvertex].end(); ++iter) {
      if (*iter > result)
        result = *iter;
    }
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
  
  std::ofstream output_file(argv[2]);
  long double result = graph.computeWeights();
  output_file << result;
  
  output_file.close();
  return 0;
}