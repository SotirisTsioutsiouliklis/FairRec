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

static bool get_options(const int argc, char ** const argv, algorithm_mode &algo_mode, std::string &pre_fix) {
    if (argc != 2) goto error;
    if (!std::strcmp(argv[1], "-gd")) {
        algo_mode = algorithm_mode::GREEDY;
        pre_fix = "greedy_per_one";
    } else if (!std::strcmp(argv[1], "-fgd")) {
        algo_mode = algorithm_mode::FAST_GREEDY;
        pre_fix = "fast_greedy_per_one";
    } else if (!std::strcmp(argv[1], "-apx")) {
        algo_mode = algorithm_mode::APPROX;
    } else if (!std::strcmp(argv[1], "-fapx")) {
        algo_mode = algorithm_mode::FAST_APPROX;
    } else if (!std::strcmp(argv[1], "-rnd")) {
        algo_mode = algorithm_mode::RANDOM;
    } else if (!std::strcmp(argv[1], "-rsrc")) {
        algo_mode = algorithm_mode::RAND_SRC;
        pre_fix = "random_source_per_one";
    } else {
        goto error;
    }
    return true;

error:
	std::cerr << "Usage: " << argv[0] << " [options]\n"
		"Options:\n"
		"-gd \t\t\t Greedy \n"
		"-fgd \t\t\t Fast Greedy\n"
		"-apx \t\t\t  Approximation\n"
		"-fapx \t\t\t Fast Approximation\n"
		"-rsrc \t\t\t Random Source nodes" << std::endl;
	return false;
}

int main (int argc, char **argv) {
    // Declare Parameters.
    algorithm_mode algo_mode;
    std::vector<int> nodes;
    std::vector<double> impact;
    std::string pre_fix, my_text;
    double val;
    int node, n_source;

    // Get Parameters.
    if (!get_options(argc, argv, algo_mode, pre_fix)) return 1;

    // Read Source Nodes.
    std::ifstream s_file(pre_fix + "_source_nodes.txt");
    while(getline(s_file, my_text)) {
        node = std::stoi(my_text);
        nodes.push_back(node);
    }
    s_file.close();

    // Read Impact.
    std::ifstream i_file("impact.txt");
    while(getline(i_file, my_text)) {
        val = std::stof(my_text);
        impact.push_back(val);
    }
    i_file.close();

    // Get number of source nodes.
    n_source = nodes.size();

    // Declare variables.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    pagerank_v rank_vector;
    std::vector<int> neighbors;
    std::vector<int>::iterator nei;
    int degree;

    // Open log file.
    std::ofstream log_file("source_node_infos.txt");
    log_file << "Node\tPagerank\tOwn_Personilized\tRed_Personilized\tOut_Degree\tOut_Red_Degree\tImpact\n";

    for (int i = 0; i < n_source; i++) {
        node = nodes[i];
        log_file << node << "\t";

        /*
        // Print out neighbors.
        neighbors = g.get_out_neighbors(node);
        std::cout << "Out neighors:\n";
        for (nei = neighbors.begin(); nei < neighbors.end(); nei++) {
            std::cout << *nei << " , ";
        }
        std::cout << std::endl;
        */
        // Get pagerank. 
        rank_vector = algs.get_pagerank();
        log_file << rank_vector[node].pagerank << "\t";
        // Get own personilized.
        rank_vector = algs.get_node_abs_prob(node);
        log_file <<rank_vector[node].pagerank << "\t";
        // Get Red personilized.
        rank_vector = algs.get_red_abs_prob();
        log_file << rank_vector[node].pagerank << "\t";
        // Get Out degree.
        degree = g.get_out_degree(node);
        log_file << degree << "\t";
        // Get Red Out Neighbors.
        degree = g.count_out_neighbors_with_community(node, 1);
        log_file << degree << "\t";
        // Get Impact.
        log_file << impact[node] << "\n";
    }

    log_file.close();

    return 1;
}