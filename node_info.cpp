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
*/

std::vector<int> get_src_nodes() {
    int node;
    std::vector<int> nodes;
    std::ifstream in_file("sources_and_edges_source_nodes.txt");
    std::string my_text;

    while (getline (in_file, my_text)) {
        node = std::stoi(my_text);
        nodes.push_back(node);
    }

    return nodes;

}

void source_node_infos() {
    // Declare Parameters.
    std::vector<int> nodes;
    std::vector<double> s1_impact, s10_impact, smax_impact;
    std::string pre_fix, my_text;
    double val;
    int node, n_source, nnodes;

    // Get Parameters.
    //if (!get_options(argc, argv, algo_mode, pre_fix)) return 1;

    // Read Number of nodes.
    std::ifstream s_file("out_graph.txt");
    s_file >> nnodes;
    nodes.resize(nnodes);
    for (int i = 0; i < nnodes; i++) {
        nodes[i] = i;
    }

    // Read s1 Impact.
    std::ifstream i_file("s1_impact.txt");
    while(getline(i_file, my_text)) {
        val = std::stof(my_text);
        s1_impact.push_back(val);
    }
    i_file.close();

    // Read s10 Impact.
    std::ifstream j_file("s10_impact.txt");
    while(getline(j_file, my_text)) {
        val = std::stof(my_text);
        s10_impact.push_back(val);
    }
    j_file.close();

    // Read smax Impact.
    std::ifstream k_file("smax_impact.txt");
    while(getline(k_file, my_text)) {
        val = std::stof(my_text);
        smax_impact.push_back(val);
    }
    k_file.close();

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
    log_file << "Node\tPagerank\tOwn_Personilized\tRed_Personilized\tOut_Degree\tOut_Red_Degree\ts1_Impact\ts10_impact\tsmax_impact\n";

    for (int i = 0; i < n_source; i++) {
        node = nodes[i];
        log_file << node << "\t";

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
        log_file << s1_impact[node] << "\t";
        log_file << s10_impact[node] << "\t";
        log_file << smax_impact[node] << "\n";
    }

    log_file.close();
}

void  get_target_to_source_stats(int src) {
    // Declare Parameters.
    std::vector<int> nodes;
    std::vector<double> t_impact;
    std::string pre_fix, my_text;
    double val;
    int node, n_source, nnodes;

    // Get Parameters.
    //if (!get_options(argc, argv, algo_mode, pre_fix)) return 1;

    // Read Number of nodes.
    std::ifstream s_file("out_graph.txt");
    s_file >> nnodes;
    nodes.resize(nnodes);
    for (int i = 0; i < nnodes; i++) {
        nodes[i] = i;
    }

    // Read s1 Impact.
    std::ifstream i_file("target_s_" + std::to_string(src) + "_impact.txt");
    while(getline(i_file, my_text)) {
        val = std::stof(my_text);
        t_impact.push_back(val);
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
    std::ofstream log_file("target_s_" + std::to_string(src) +"_node_infos.txt");
    log_file << "Node\tPagerank\tto_src_Personilized\tRed_Personilized\tOut_Degree\tOut_Red_Degree\tt_Impact\n";

    for (int i = 0; i < n_source; i++) {
        node = nodes[i];
        log_file << node << "\t";

        // Get pagerank. 
        rank_vector = algs.get_pagerank();
        log_file << rank_vector[node].pagerank << "\t";
        // Get own personilized.
        rank_vector = algs.get_node_abs_prob(node);
        log_file <<rank_vector[src].pagerank << "\t";
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
        log_file << t_impact[node] << "\n";
    }

    log_file.close();
}

void target_node_infos() {
    // Get source nodes.
    std::vector<int> src_nodes = get_src_nodes();
    // Get number of source nodes.
    int n_source = src_nodes.size();
    // For each source node.
    for (int s_node = 0; s_node < n_source; s_node++) {
        get_target_to_source_stats(src_nodes[s_node]);
    }

}

int main () {
    source_node_infos();
    target_node_infos();
    return 1;
}