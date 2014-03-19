#pragma once
#include <string>
#include <map>
#include <set>
#include "tree_element.h"
#include "ivisitor.h"
#include "leaf.h"
#include "node.h"

#include "fasta_data.h"
#include "computations.h"




class GuideTree {
  FastaData * fasta_data;//contains link to fasta array
  std::vector<std::shared_ptr<TreeElement> > _nodes;
  std::set<int> current_node_indices;
  
  std::map<std::pair<int, int>, double, pair_less_comparator> pair_distances;
 

  std::map<int, int> precomputed_sums;
public:
  GuideTree(FastaData * fasta): fasta_data(fasta) {  }

  //method prepares Leaf objects for future usage
  void PrepareLeaves() {
    for (int i = 0; i < fasta_data->data.size(); i++) {
      _nodes.push_back(std::shared_ptr<TreeElement>(new Leaf(i)));
      current_node_indices.insert(i);
    }

    for (int i = 0; i < fasta_data->data.size(); i++) {
      // fasta_data->data[i].second
      for (int j = i + 1; j < fasta_data->data.size(); j ++) {
        int dist = levenshtein_distance(fasta_data->data[i].second, fasta_data->data[j].second);
        precomputed_sums[i] += dist;
        precomputed_sums[j] += dist;
        pair_distances[std::make_pair(i, j)] = dist;
      }
      
    }
  }
  double getDistance(int i, int j) {
    if (i > j)
      return getDistance(j, i);
    if (i == j)
      return -1;
    return pair_distances[std::make_pair(i, j)];
  }

  std::pair<int, int> getMinPairFromQMatrix() {
    std::map<std::pair<int, int>, int> qvalues;
    int min = -1;//i know that magic numbering is evil, but..
    std::pair<int, int> min_value;
    //recompute sums: at this point let's recompute them every time we build matrix
    for (std::set<int>::iterator i = current_node_indices.begin(); i != current_node_indices.end(); ++i) {
      precomputed_sums[*i] = 0;
      for (std::set<int>::iterator j = current_node_indices.begin(); j != current_node_indices.end(); ++j) {
        if (*i != *j)
          precomputed_sums[*i] += getDistance(*i, *j);
      }
    }
   
    for (std::set<int>::iterator i = current_node_indices.begin(); i != current_node_indices.end(); ++i) {
      for (std::set<int>::iterator j = current_node_indices.begin(); j != current_node_indices.end(); ++j) {
        if (*i >= *j)
          continue;
        std::pair<int, int> current_pair = std::pair<int, int>(*i, *j);
   
        if (i != j) {
          qvalues[current_pair] = current_node_indices.size()*this->getDistance(*i, *j) - precomputed_sums[*i] - precomputed_sums[*j]; 
          if (min == -1 || qvalues[current_pair] < min) {
            min = qvalues[current_pair];
            min_value = std::pair<int, int>(*i, *j);
          }
        }
      }
    }
    return min_value;
  }

  void Build() {
    PrepareLeaves();
    //в цикле:
    //1. строим Q, возвращаем пару вершин до которых минимальное расстояние
    for (int i = 0 ; i < fasta_data->data.size() - 1; i++) {
      std::pair<int, int> min_pair = getMinPairFromQMatrix();
      //3. добавляем новую вершину, являющуюся объединением старых, в список
      int u = _nodes.size();
      _nodes.push_back(std::shared_ptr<TreeElement>(new Node(u, _nodes[min_pair.first], _nodes[min_pair.second])));
      pair_distances[std::pair<int, int>(min_pair.first, u)] = 0.5*(getDistance(min_pair.first, min_pair.second) +
                (precomputed_sums[min_pair.first] - precomputed_sums[min_pair.second])/(u - 2));
      pair_distances[std::pair<int, int>(min_pair.second, u)] = getDistance(min_pair.first, min_pair.second) - 
                pair_distances[std::pair<int, int>(min_pair.first, u)];
      current_node_indices.erase(current_node_indices.find(min_pair.first));
      current_node_indices.erase(current_node_indices.find(min_pair.second));
      for (std::set<int>::iterator iter = current_node_indices.begin(); iter != current_node_indices.end(); ++iter) {
        pair_distances[std::pair<int, int>(*iter, u)] = (getDistance(min_pair.first, *iter) + getDistance(min_pair.second, *iter) - getDistance(min_pair.first, min_pair.second))/2;
      }
      current_node_indices.insert(u);
    }
    //удаляем вершины из списка
    //пересчитываем расстояние до новой вершины
    //а также сумму расстояний от нее до остальных узлов в списке
  }

  void acceptVisitor(IVisitor* visitor) {
    for (std::set<int>::iterator iter = current_node_indices.begin(); iter != current_node_indices.end(); ++iter) {
      (_nodes[*iter])->acceptVisitor(visitor);
    }
  }
};