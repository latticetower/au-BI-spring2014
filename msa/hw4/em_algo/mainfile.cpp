#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include "fasta_data.h"

long double pi1 = log2l(0.996),
       pi2 = log2l(0.004);
long double a11 = log2l(0.999),
       a12 = log2l(0.001),
       a21 = log2l(0.01),
       a22 = log2l(0.99);
long double e1_at = log2l(0.291),
       e1_cg = log2l(0.209),
       e2_at = log2l(0.169),
       e2_cg = log2l(0.331);


//helper function
long double log2l_sum(long double log2lp, long double log2lq) {
  //if (log2lp < log2lq)
    //return log2l_sum(log2lq, log2lp);
  return log2lp + log2l(1.0 + exp2(log2lq - log2lp));
  //return log2l(exp2(log2lp)+exp2(log2lq));
}


long double getE(int t, char c) {
  if (t == 1) {
    if (c == 'A' || c == 'T')
      return e1_at;
    else return e1_cg;
  }
  else {
    if (c == 'A' || c == 'T')
      return e2_at;
    return e2_cg;
  }
}

std::vector<std::pair<long double, long double> > alpha_values;

long double alpha_base = 0;

std::pair<long double, long double> forward_init(char chr){
  alpha_values.clear();
  alpha_base = 0;
  long double alpha1 = getE(1, chr) + pi1;
  long double alpha2 = getE(2, chr) + pi2;
  alpha_base += log2l_sum(alpha1, alpha2);
  //alpha1 = alpha1 - log2l_sum(alpha1, alpha2);
  //alpha2 = alpha2 - log2l_sum(alpha1, alpha2);
  alpha_values.push_back(std::make_pair(alpha1, alpha2));
  return std::make_pair(alpha1, alpha2);
}

std::pair<long double, long double> forward_step(char chr, std::pair<long double, long double> alpha_prev) {
  long double alpha1, alpha2;

  alpha1 = getE(1, chr) + log2l_sum(alpha_prev.first + a11, alpha_prev.second + a21);
  alpha2 = getE(2, chr) + log2l_sum(alpha_prev.first + a12, alpha_prev.second + a22);

  alpha_base += log2l_sum(alpha1, alpha2);
  //alpha1 = alpha1 - log2l_sum(alpha1, alpha2);
  //alpha2 = alpha2 - log2l_sum(alpha1, alpha2);

  alpha_values.push_back(std::make_pair(alpha1, alpha2));
  return std::make_pair(alpha1, alpha2);
}

std::vector<std::pair<long double,long double> > beta_values;

std::pair<long double, long double> backward_init(size_t max_size) {
  beta_values.clear();
  std::pair<long double, long double> beta = std::make_pair(log2l(1.0), log2l(1.0));
  //i know there must be zeroes, this is just for clearance
  beta_values.push_back(beta);
  return beta;
}


std::pair<long double, long double> backward_step(std::pair<long double, long double> beta_next, char chr) {
  long double beta1, beta2;
  beta1 = log2l_sum(beta_next.first  + a11 + getE(1, chr),
                    beta_next.second + a12 + getE(2, chr));
  beta2 = log2l_sum(beta_next.first  + a21 + getE(1, chr),
                    beta_next.second + a22 + getE(2, chr));
  std::pair<long double, long double> beta = std::make_pair(beta1, beta2);
  beta_values.push_back(beta);
  return beta;
}

std::vector<std::pair<long double, long double> > gamma_values;
std::vector<std::pair<std::pair<long double, long double>, std::pair<long double, long double> > > xi_values;

void compute_gamma_xi(std::string const& str) {
  gamma_values.clear();
  xi_values.clear();
  long double gamma1, gamma2, den;
  for (size_t i = 0; i< alpha_values.size(); i++) {
    gamma1 = (alpha_values[i].first + beta_values[i].first);
    gamma2 = (alpha_values[i].second + beta_values[i].second);
    den = log2l_sum(gamma1, gamma2);
    gamma1 = gamma1 - den;
    gamma2 = gamma2 - den;

    gamma_values.push_back(std::make_pair(gamma1, gamma2));
    //xi
  }
  //std::cout << alpha_values[1].first << alpha_values[1].second << std::endl;
  //std::cout << beta_values[0].first << beta_values[0].second << std::endl;
  //std::cout << "gamma: " << gamma_values.front().first << " " << gamma_values.front().second << std::endl;
  long double xi11, xi12, xi21, xi22;
  for (size_t i = 0; i < alpha_values.size() - 1; i++) {
    xi11 = alpha_values[i].first + a11 + beta_values[i + 1].first + getE(1, str[i + 1]);
    xi12 = alpha_values[i].first + a12 + beta_values[i + 1].second + getE(2, str[i + 1]);
    xi21 = alpha_values[i].second + a21 + beta_values[i + 1].first + getE(1, str[i + 1]);
    xi22 = alpha_values[i].second + a22 + beta_values[i + 1].second + getE(2, str[i + 1]);
    den = log2l_sum(log2l_sum(xi11, xi12), log2l_sum(xi21, xi22));
    xi11 = xi11 - den;
    xi12 = xi12 - den;
    xi21 = xi21 - den;
    xi22 = xi22 - den;

    std::pair <std::pair<long double, long double>, std::pair<long double, long double> > xi = std::make_pair(std::make_pair(xi11, xi12), std::make_pair(xi21, xi22));
    xi_values.push_back(xi);
  }
  //std::cout << "xi: " << xi11 << " " << xi12 << " " << xi21 << " " << xi22 << std::endl;

}

long double get_state_sequence(std::string const& str, std::string& target) {
  std::pair<long double, long double> alpha = forward_init(str[0]);

  for (size_t i = 1; i < str.size(); i++) {
    alpha = forward_step(str[i], alpha);   //std::cout << val.first << " " << val.second <<std::endl;
  }
  //std::string str2 = ""
  //std::cout <<"\n backward\n";
  std::pair<long double, long double> beta = backward_init(str.size());

  //std::cout << x.first << " " <<x.second <<std::endl;
  //target[str.size() - 1] = (x.first == 1? '1' : '2');
  for (size_t i = 1; i < str.size(); i++) {
    beta = backward_step(beta, str[str.size() - i]);
    //std::cout << x.first << " " <<x.second <<std::endl;
    //target[x.second] = (x.first == 1? '1' : '2');
  }
  //std::cout.setf(std::ios::fixed|std::ios::showpoint);
  //std::cout << beta.first << " " << beta.second << std::endl;

  //reverse
  std::reverse(beta_values.begin(), beta_values.end());
  compute_gamma_xi(str);

//output
//std::cout << "alphas: " << alpha_values.back().first << " " << alpha_values.back().second << std::endl
//          << "betas: " << beta_values.front().first << " " << beta_values.front().second << std::endl
//          << "gammas: " << gamma_values.back().first << " " << gamma_values.back().second << std::endl
//          << "xi : " << xi_values.back().first << " " << xi_values.back().second<<std::endl;
  //std::cout << x_values.size() << " " <<v_values.size() << " " <<str.size() << " - " << "dddd";
  return log2l_sum(alpha_values.back().first, alpha_values.back().second);
}


void update_data(std::string const & str, std::string const & states_string) {
  std::cout.precision(10);

  pi1 = gamma_values[0].first;
  pi2 = gamma_values[0].second;
  //std::cout << "pi: " << exp2(pi1) << " " << exp2(pi2) << std::endl;
  long double g1, g2, x11,x12, x21, x22;
  g1 = gamma_values[0].first;
  g2 = gamma_values[0].second;
  x11 = xi_values[0].first.first;
  x12 = xi_values[0].first.second;
  x21 = xi_values[0].second.first;
  x22 = xi_values[0].second.second;
  //
  for (size_t i = 1; i < states_string.size()-1; i++) {
    g1 = log2l_sum(g1, gamma_values[i].first);
    g2 = log2l_sum(g2, gamma_values[i].second);
    x11 = log2l_sum(x11, xi_values[i].first.first);
    x12 = log2l_sum(x12, xi_values[i].first.second);
    x21 = log2l_sum(x21, xi_values[i].second.first);
    x22 = log2l_sum(x22, xi_values[i].second.second);
  }

  a11 = x11 - g1;
  a12 = x12 - g1;
  a21 = x21 - g2;
  a22 = x22 - g2;
  //std::cout << "a: " << exp2(a11) << " " << exp2(a12) << " " << exp2(a21) << " " << exp2(a22) << std::endl;

  bool cg_set = false, at_set = false;
  //recompute emissions
  if (str[0]=='A' || str[0] == 'T') {
    e1_at = gamma_values[0].first;
    e2_at = gamma_values[0].second;
    at_set = true;
    e1_cg = 0;//log2l 1
    e2_cg = 0;//log2l 1
  }
  else {
    cg_set = true;
    e1_at = 0;//log2l 1
    e2_at = 0;//log2l 1
    e1_cg = gamma_values[0].first;
    e2_cg = gamma_values[0].second;
  }
  for(size_t i = 1; i < str.size(); i++ ) {
    if (str[i] == 'A' || str[i] == 'T') {
      if (at_set) {
        e1_at = log2l_sum(e1_at, gamma_values[i].first);
        e2_at = log2l_sum(e2_at, gamma_values[i].second);
      }
      else {
        at_set = true;
        e1_at = gamma_values[i].first;
        e2_at = gamma_values[i].second;
      }
    }
    else {
      if (cg_set) {
        e1_cg = log2l_sum(e1_cg, gamma_values[i].first);
        e2_cg = log2l_sum(e2_cg, gamma_values[i].second);
      }
      else {
        cg_set = true;
        e1_cg = gamma_values[i].first;
        e2_cg = gamma_values[i].second;
      }
    }
  }
  long double den = log2l(2) + log2l_sum(e1_at, e1_cg);
  e1_at = e1_at - den;
  e1_cg = e1_cg - den;
  den = log2l(2) + log2l_sum(e2_at, e2_cg);
  e2_at = e2_at - den;
  e2_cg = e2_cg - den;
  //std::cout << "e1: " << exp2(e1_at) << " " << exp2(e1_cg) << std::endl;
  //std::cout << "e2: " << exp2(e2_at) << " " << exp2(e2_cg) << std::endl;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Please, provide input file name as a first parameter" << std::endl;
    return 0;
  }
  std::ifstream inputStream(argv[1]);
  if (inputStream.fail()) {
    std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
    return 0;
  }
  FastaData fasta;
  fasta.loadFromStream(inputStream);
  inputStream.close();
  std::string str1 = fasta.data[0].second;
  std::string s(str1.c_str());
  std::ofstream output_file1("em_results.txt");
  output_file1 << argv[1] << std::endl << fasta.data[0].first << std::endl;
  output_file1.precision(5);
  output_file1.setf(std::ios::scientific);
  long double log2l_likelihood_old = 0,
              log2l_likelihood_new = 0;
  int counter = 0;
  while (true) {
    counter++;
    if (counter > 1) {
      log2l_likelihood_old = log2l_likelihood_new;
    }
    std::cout << "Iteration "<< counter <<std::endl;
    log2l_likelihood_new = get_state_sequence(str1, s);
    std::cout << " new likelihood: "<< log2l_likelihood_new;
    if (counter > 1) {
      std::cout << " diff= " << log2l_likelihood_new - log2l_likelihood_old;
    } std::cout <<std::endl;
    update_data(str1, s);
    if (counter > 1) {
      if (log2l_likelihood_new - log2l_likelihood_old < 0.0) {
        std::cout << "got some log2l problems\n";
        return 1;
      } else
        if (log2l_likelihood_new - log2l_likelihood_old < (0.1))
          break;
    }

  }
  output_file1 << "Baum-Welch training: " <<counter <<" iterations passed\n final log-likelihood is " << log2l_likelihood_new << std::endl;

//  output_file1.setf(std::ios::scientific);
  output_file1  << " New initial probabilities are: pi1=" << exp2(pi1) << ", pi2=" << exp2(pi2) << std::endl;

  output_file1  << " New transitional probabilities: a11=" << exp2(a11) << ", a12=" << exp2(a12) << ", a21="
      << exp2(a21) << ", a22=" << exp2(a22) << std::endl;

  output_file1  << " New emissions are: e1_at=" << exp2(e1_at) << ", e1_cg="
      << exp2(e1_cg) << ", e2_at=" << exp2(e2_at) << ", e2_cg=" << exp2(e2_cg) << std::endl;

  //std::cout << s;
  output_file1.close();
  return 0;
}
