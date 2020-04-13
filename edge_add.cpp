#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "edge_add.hpp"

Edge_addition::Edge_addition(graph &g, pagerank_algorithms &algs, int n_source) {
    g = g;
    algs = algs;
    n_source = n_source;
}


