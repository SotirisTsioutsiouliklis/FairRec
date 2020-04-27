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

/*
static bool get_options(const int argc, char ** const argv, int &node) {
    if (argc != 3) goto error;
    if ( !std::strcmp(argv[1], "-n") ) {
        node = std::atoi(argv[2]);
    }
    return true;

error:
    std::cerr << "Usage: " << argv[0] << " [options]\n"
		"Options:\n"
        "-n <node id>\n";
    return false;
}
*/

int main () {
    // Declare parameters.
    //int node; // Node id.

    // Get parameters.
    //if (!get_options(argc, argv, node)) return 1;

    // Make array as argument.
    int nodes[] = {759, 812, 749, 786};

    // Declare variables.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    pagerank_v rank_vector;
    std::vector<int> neighbors;
    std::vector<int>::iterator nei;
    int degree;

    // Open log file.
    std::ofstream log_file("source_node_infos.txt");
    log_file << "Node\tPagerank\tOwn Personilized\tRed Personilized\tOut Degree\tOut Red Degree\n";

    for (int i = 0; i < 4; i++) {
        log_file << nodes[i] << "\t";

        // Print out neighbors.
        neighbors = g.get_out_neighbors(nodes[i]);
        std::cout << "Out neighors:\n";
        for (nei = neighbors.begin(); nei < neighbors.end(); nei++) {
            std::cout << *nei << " , ";
        }
        std::cout << std::endl;
        
        // Get pagerank. 
        rank_vector = algs.get_pagerank();
        log_file << rank_vector[nodes[i]].pagerank << "\t";

        // Get own personilized.
        rank_vector = algs.get_node_abs_prob(nodes[i]);
        log_file <<rank_vector[nodes[i]].pagerank << "\t";

        // Get Red personilized.
        rank_vector = algs.get_red_abs_prob();
        log_file << rank_vector[nodes[i]].pagerank << "\t";

        // Get Out degree.
        degree = g.get_out_degree(nodes[i]);
        log_file << degree << "\t";

        // Get Red Out Neighbors.
        degree = g.count_out_neighbors_with_community(nodes[i], 1);
        log_file << degree << "\n";  
    }

    log_file.close();

    return 1;
}