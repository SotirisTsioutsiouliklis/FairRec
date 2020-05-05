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



static bool get_options(const int argc, char ** const argv, algorithm_mode &algo_mode, int &n_source, int &n_target, int &n_edges, 
    source_criterion &s_criterion, target_criterion &t_criterion, edge_criterion &e_criterion, prediction_kind &p_kind, log_kind &l_kind) {
    if (argc != 6 && argc != 2) goto error;
    if (!std::strcmp(argv[1], "-gd") && !std::strcmp(argv[2], "-s") && !std::strcmp(argv[4], "-t")) {
        algo_mode = algorithm_mode::GREEDY;
        n_source = std::atoi(argv[3]);
        n_target = std::atoi(argv[5]);
    } else {
        goto error;
    }
    return true;

error:
	std::cerr << "Usage: " << argv[0] << " [options]\n"
		"Options:\n"
		"-gd -s <number of source nodes> -t <number of target nodes>\t\t\t Greedy \n"
		"-fgd -s <number of source nodes> -t <number of target nodes>\t\t\t Fast Greedy\n"
		"-apx -s <number of source nodes> -t <number of target nodes>\t\t\t  Approximation\n"
		"-fapx -s <number of source nodes> -t <number of target nodes>\t\t\t Fast Approximation\n"
		"-rsrc -s <number of source nodes> -t <number of target nodes>\t\t\t Random Source nodes\n"
		"-srcst -s <number of source nodes> -t <number of target nodes>\t\t\t Sources Statistics\n"
		"-tarst -s <number of source nodes> -t <number of target nodes>\t\t\t Target Statistics\n"
		"-ota \t\t\t One to All" << std::endl;
	return false;
}

int main(int argc, char **argv) {
    // Init arguments.
    algorithm_mode algo_mode;
    source_criterion s_criterion;
    target_criterion t_criterion;
    edge_criterion e_criterion;
    prediction_kind p_kind;
    log_kind l_kind;
    int n_source = 1;
    int n_target = 1;
    int n_edges = 1;
    int option;

    // Edit with arguments for automization.
    // if (!get_options(argc, argv, algo_mode, n_source, n_target, n_edges, s_criterion, t_criterion, e_criterion, p_kind, l_kind)) return 1;

    // Init graph and algorithms.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    Edge_addition link_rec(g, algs, n_source, n_target);

    // Ask user for an option.
    std::cout << "Options:\n"
            "1.\tYou will choose the number of the edges. All edges will be added to the best source in a greedy way.\n"
            "2.\tYou will choose the number of the edges. All edges will be added randomly\n"
            "3.\tYou will choose the number of the edges. All edges will be added based to edge criterion one\n"
            "Insert option: ";
}