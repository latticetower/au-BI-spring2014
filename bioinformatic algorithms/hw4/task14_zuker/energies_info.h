#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>

template<typename to, typename from>
to lexical_cast(from const &x) {
  std::stringstream os;
  to ret;
  os << x;
  os >> ret;
  return ret;  
}

class EnergiesInfo {

  public:
    static const int INF = 10000;

    enum RNANucleotide{A, C, G, U};
    enum Structure{Bulge, Hairpin, InternalLoop, None, BulgeOrLoop, Stacked, Composed, ComposedVM, Simple};

    void LoadFromFile(std::string const&filename) {
      std::ifstream f(filename);
      load_free_energies(f);
      load_structure_energies(f);
      f.close();
    }

    // methods for getting specific portion of energies
    double get(Structure s, int i) const {
      std::map<Structure, std::map<int, double> >::const_iterator inner_map = structure_energies.find(s);
      if (inner_map == structure_energies.end())
        return INF;
      if (inner_map->second.find(i) == inner_map->second.end())
        return INF;
      return structure_energies.at(s).at(i);
    }

    double get(char a1, char a2, char b1, char b2) const {
      std::map<NucleotidePair, std::map<NucleotidePair, double> >::const_iterator iter = free_energies.find(getNucleotidePair(a1, a2));
      if (iter == free_energies.end()) 
        return INF;
      if (iter->second.find(getNucleotidePair(b1, b2)) == iter->second.end()) 
        return INF;
      return iter->second.at(getNucleotidePair(b1, b2));
    }

  protected:
    bool load_free_energies(std::istream & is) {
      free_energies.clear();
      std::string header, line;
      std::getline(is, header);
      std::vector<std::string> parsed_header = parse(header, '\t');
      for (int i = 0; i < 6; i++) {
        std::getline(is, line);
        std::vector<std::string> parsed_line = parse(line, '\t');
        NucleotidePair np1 = getNucleotidePair(parsed_line[0]);
        free_energies[np1] = std::map<NucleotidePair, double>();
        for (int j = 0; j < 6; j++){
          NucleotidePair np2 = getNucleotidePair(parsed_header[j + 1]);
          free_energies[np1][np2] = lexical_cast<double, std::string>(parsed_line[j + 1]);
        }
      }
      return true;
    }

    bool load_structure_energies(std::istream & is) {
      structure_energies.clear();
      structure_energies[Structure::Bulge] = std::map<int, double>();
      structure_energies[Structure::Hairpin] = std::map<int, double>();
      structure_energies[Structure::InternalLoop] = std::map<int, double>();
      std::string line;
      std::getline(is, line);
      std::vector<std::string> parsed_line = parse(line, '\t');
      while (!is.eof() && parsed_line.size() == 4) {
        std::getline(is, line);
        parsed_line = parse(line, '\t');
        int size = lexical_cast<int, std::string>(parsed_line[0]);
        structure_energies[Structure::InternalLoop][size] = lexical_cast<double, std::string>(parsed_line[1]);
        structure_energies[Structure::Bulge][size] = lexical_cast<double, std::string>(parsed_line[2]);
        structure_energies[Structure::Hairpin][size] = lexical_cast<double, std::string>(parsed_line[3]);
      }
      return true;
    }

    
  protected:

    std::vector<std::string> parse(std::string const &str, char delimiter) {
      std::vector<std::string> result;
      auto start = 0U;
      auto end = str.find(delimiter);
      while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + sizeof(delimiter);
        end = str.find(delimiter, start);
      }
      result.push_back(str.substr(start, end));
      return result;
    }
    
   
  private:
    typedef std::pair<RNANucleotide, RNANucleotide> NucleotidePair;

    NucleotidePair getNucleotidePair(std::string const& s) const {
      return getNucleotidePair(s[0], s[2]);
    }
    NucleotidePair getNucleotidePair(char a, char b) const {
      return std::make_pair(getNucleotide(a), getNucleotide(b));
    }

    RNANucleotide getNucleotide(char ch) const {
      if (ch == 'a' || ch == 'A')
        return RNANucleotide::A;
      if (ch == 'c' || ch == 'C')
        return RNANucleotide::C;
      if (ch == 'g' || ch == 'G')
        return RNANucleotide::G;
      if (ch == 'u' || ch == 'U')
        return RNANucleotide::U;
    }


    
    std::map<NucleotidePair, std::map<NucleotidePair, double> > free_energies;
    std::map<Structure, std::map<int, double> > structure_energies;
};