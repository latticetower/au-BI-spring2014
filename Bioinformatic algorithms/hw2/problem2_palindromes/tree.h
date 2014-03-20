#pragma once
#include <memory>
#include <vector>

struct Node{
  int left; //[
  int right;//)
  int value;
  Node* leftChild, *rightChild; 
  Node(int l, int r, int value){}
};
class Tree{
  std::shared_ptr<Node> root;
public: 
  Tree() {
  }
  void Build(std::vector<int> a, std::vector<int> b) {


  }
};