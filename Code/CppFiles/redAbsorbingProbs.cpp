/**
 * Debug red absorbing probs.
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include "graph.hpp"
#include "pagerank.hpp"
#include "edgeAddition.hpp"
#include <chrono>
#include <omp.h>

int main() {
    // Define number of threads to use.
    omp_set_num_threads(20);
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::cout << "Calculating red Abs Probs\n";
    pagerank_v redProbs = algs.get_red_abs_prob();
    EdgeAddition::saveVector("redProbs.txt", redProbs);
}