#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <set>
#include <map>

struct Graph{
  int n;//vertices count;
  std::map<int, std::set<int> > adj_list;

  Graph() {}

  void loadAdjList(std::istream & input_stream) {
    input_stream >> n;
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

  std::pair<std::set<int>, std::set<int> > get2Coloring() {
    std::pair<std::set<int>, std::set<int> > result;
    if (adj_list.size() == 0)
      return result;
    int startvertex = (adj_list.begin())->first;
    result.first.insert(startvertex);
    std::stack<int> observed_vertices;
    observed_vertices.push(startvertex);
    while (!observed_vertices.empty()) {
      int current_vertex = observed_vertices.top();
      observed_vertices.pop();
      for (std::set<int>::iterator iter = adj_list[current_vertex].begin(); iter != adj_list[current_vertex].end(); ++iter) {
        if (result.first.find(*iter) == result.first.end() && result.second.find(*iter) == result.second.end()) {
          if (result.first.find(current_vertex) != result.first.end()) {
            result.second.insert(*iter);
          } else if (result.second.find(current_vertex) != result.second.end()) {
            result.first.insert(*iter);
          }
          observed_vertices.push(*iter);
        } 
      }
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
  std::pair<std::set<int>, std::set<int> > coloring = graph.get2Coloring();
  
  std::ofstream output_file(argv[2]);
  if (coloring.first.size() > coloring.second.size()) {
    output_file << coloring.first.size() << std::endl;
    for (std::set<int>::iterator iter = coloring.first.begin(); iter != coloring.first.end(); ++iter) {
      output_file << *iter << " ";
    }
  } else {
    output_file << coloring.second.size() << std::endl;
    for (std::set<int>::iterator iter = coloring.second.begin(); iter != coloring.second.end(); ++iter) {
      output_file << *iter << " ";
    }
  }
  
  output_file.close();
  return 0;
}