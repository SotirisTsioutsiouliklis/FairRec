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
#include "edge_add.hpp"

void save_vector(std::string name, pagerank_v vec) {
    std::ofstream vec_file(name + ".txt");
    int n = vec.size();
    for (int i = 0; i < n; i++) {
        vec_file << vec[i].pagerank << "\n";
    }
}

int main() {
    // Declare variables.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    pagerank_v rank_vec;

    // Get Pagerank Vector.
    rank_vec = algs.get_pagerank();
    //save Pagerank Vector.
    save_vector("pagerank", rank_vec);
    // Get Red Pagerank Ratio Vector.
    rank_vec = algs.get_red_abs_prob();
    save_vector("red_ratio", rank_vec);

}