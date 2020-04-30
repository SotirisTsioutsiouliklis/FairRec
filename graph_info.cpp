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

enum class stats_mode {GRAPH, NODE};

static bool get_options(const int argc, char ** const argv, int &node, stats_mode &mode) {
    if (argc != 1 && argc != 3) goto error;
    if (argc == 3) {
        if (!std::strcmp(argv[1], "-n")) {
            mode = stats_mode::NODE;
            node = std::atoi(argv[2]);
        } else {
            goto error;
        }
    }

    return true;

error:
	std::cerr << "Usage: " << argv[0] << " [options]\n"
		"Options:\n"
		"./graph_info.out\t\t\t Pagerank & Red Absorbing Probabilities.\n"
		"./graph_info.out -n <node>\t\t\t Node Absorbing Probabilities." << std::endl;
	return false;
}

void save_vector(std::string name, pagerank_v vec) {
    std::ofstream vec_file(name + ".txt");
    int n = vec.size();
    for (int i = 0; i < n; i++) {
        vec_file << vec[i].pagerank << "\n";
    }
}

int main(int argc, char **argv) {
    // Parameters.
    int node;
    stats_mode mode = stats_mode::GRAPH;
    // Get Parameters.
    if (!get_options(argc, argv, node, mode)) return 1;

    // Declare variables.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    pagerank_v rank_vec;

    switch (mode)
    {
    case stats_mode::GRAPH:
        // Get Pagerank Vector.
        rank_vec = algs.get_pagerank();
        //save Pagerank Vector.
        save_vector("pagerank", rank_vec);
        // Get Red Pagerank Ratio Vector.
        rank_vec = algs.get_red_abs_prob();
        save_vector("red_ratio", rank_vec);
        break;
    case stats_mode::NODE:
        // Get Source Pagerank Ratio Vector.
        rank_vec = algs.get_node_abs_prob(node);
        save_vector("node_" + std::to_string(node) + "_ratio", rank_vec);
        break;
    default:
        std::cout << "No Valide Option!\n";
        break;
    }
}