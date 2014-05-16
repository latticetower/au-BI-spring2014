#pragma once
#include <fstream>
#include <string>
#include <memory>
#include <map>
#include "newick_lexer.h"
#include "multi_string.h"
#include"fasta_data.h"

struct tree_vertex {
  std::string name;
  std::shared_ptr<tree_vertex> left_child;
  std::shared_ptr<tree_vertex> right_child;
  MultiString sequence, 
    selected_string;
  long score;

  tree_vertex() {
    name = "";
    score = 0;
    left_child = NULL;
    right_child = NULL;
  }

  tree_vertex(tree_vertex& orig) {
    this->name = orig.name;
    this->left_child = orig.left_child;
    this->right_child = orig.right_child;
    this->sequence = orig.sequence;
    this->score = orig.score;
  }
  tree_vertex(std::string const &_name, tree_vertex*left, tree_vertex*right): name(_name), left_child(left), right_child(right) {
    this->score = 0;
  }

  void load_sequence(std::string const& str) {
    sequence.load_from_string(str);
  }

  bool is_leaf() {
    return (left_child == NULL && right_child == NULL);
  }
  
  void traverse_top_down() {
    if (this->is_leaf())
      return;
    this->selected_string.select_sequence();
    if (left_child != NULL) {
      left_child->selected_string = left_child->sequence;
      left_child->selected_string = left_child->sequence & this->selected_string;
      left_child->traverse_top_down();
    }
    if (right_child != NULL) {
      right_child->selected_string = right_child->sequence & this->selected_string;
      right_child->traverse_top_down();
    }
  }
  
  void construct_sets() {
    if (this->is_leaf())
      return;
    this->sequence.clear();
    this->score = 0;
    if (left_child != NULL) { 
      left_child->construct_sets();
      this->sequence = left_child->sequence;
      this->score = left_child->score;
    }
    if (right_child != NULL) {
      right_child->construct_sets();
      this->sequence = this->sequence + right_child->sequence;
      this->score = this->score + right_child->score;
    }
    
    this->score = this->score + this->sequence.getScore();
  }
};

class BinaryRootedTree {
  NewickLexer lexer;
  std::shared_ptr<tree_vertex> root;
  std::map<std::string, tree_vertex *> nodes_by_names;

  tree_vertex* loadWithChildren(NewickLexer* lexer) {
    if (lexer->peek().first == LBRACKET) {
      lexer->next();
      tree_vertex* left = loadWithChildren(lexer);
      tree_vertex* right = NULL;
      if (lexer->peek().first != RBRACKET) {
        right = loadWithChildren(lexer);
      }
      lexer->next();//should be RBRACKET
      std::pair<Token, std::string> info = lexer->peek();//NAME
      if (info.first == NAME) {
        lexer->next();
        tree_vertex * temp = new tree_vertex(info.second, left, right);
        nodes_by_names[info.second] = temp;
        return temp;
      }
    }
    else {
      std::pair<Token, std::string> info = lexer->peek();
      if (info.first == NAME) {
        lexer->next();
        tree_vertex * temp = new tree_vertex(info.second, NULL, NULL);
        nodes_by_names[info.second] = temp;
        return temp;
      }
    }
    return new tree_vertex();
  }
  
  void reloadFromString() {
    this->root = std::shared_ptr<tree_vertex>(loadWithChildren(&lexer)); 
  }
public:
  BinaryRootedTree() {
  }
  
  FastaData getInnerNodeSequences() {
    FastaData data;
    for (std::map<std::string, tree_vertex*>::iterator iter = nodes_by_names.begin(); iter!= nodes_by_names.end(); ++iter) {
      if (!iter->second->is_leaf()) 
        data.insertPair(iter->first, iter->second->selected_string.to_s());
    }
    return data;
  }
  void loadSequencesFromFasta(FastaData * fasta) {
    for (std::vector<std::pair<std::string, std::string> >::iterator iter = fasta->data.begin(); iter != fasta->data.end(); ++iter) {
      if (nodes_by_names.find(iter->first) != nodes_by_names.end()) {
        nodes_by_names[iter->first]->load_sequence(iter->second);
      }
    }
  }

  void loadStructure(std::istream & is) {
    std::string newick_string;
    std::getline(is, newick_string);
    lexer.Build(newick_string);
    reloadFromString();
  }

  void traverseTopDown() {
    root->selected_string = root->sequence;
    root->traverse_top_down();
  } 
  
  void constructSets(){
    root->construct_sets();
  }
  long getScore() {
    return root->score;
  }
};