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



static bool get_options(const int argc, char ** const argv, algorithm_mode &algo_mode, int &n_source, int &n_target) {
    if (argc != 6) goto error;
    if (!std::strcmp(argv[1], "-gd") && !std::strcmp(argv[2], "-s") && !std::strcmp(argv[4], "-t")) {
        algo_mode = algorithm_mode::GREEDY;
        n_source = std::atoi(argv[3]);
        n_target = std::atoi(argv[5]);

    } else if (!std::strcmp(argv[1], "-fgd") && !std::strcmp(argv[2], "-s") && !std::strcmp(argv[4], "-t")) {
        algo_mode = algorithm_mode::FAST_GREEDY;
        n_source = std::atoi(argv[3]);
        n_target = std::atoi(argv[5]);

    } else if (!std::strcmp(argv[1], "-apx") && !std::strcmp(argv[2], "-s") && !std::strcmp(argv[4], "-t")) {
        algo_mode = algorithm_mode::APPROX;
        n_source = std::atoi(argv[3]);
        n_target = std::atoi(argv[5]);

    } else if (!std::strcmp(argv[1], "-fapx") && !std::strcmp(argv[2], "-s") && !std::strcmp(argv[4], "-t")) {
        algo_mode = algorithm_mode::FAST_APPROX;
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
		"-fapx -s <number of source nodes> -t <number of target nodes>\t\t\t Fast Approximation" << std::endl;
	return false;
}

void check_precision_effect(Edge_addition &link_rec) {
    // Declare local variables.
    int log_s = link_rec.get_log_size();
    double presicions [11] = {1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9, 1e-10, 1e-11, 1e-12, 1e-13, 1e-14};
    //double presicions [1] = {1e-14};
    int iterations [11] = {100, 200, 400, 800, 1200, 1600, 2000, 2400, 2800, 3000, 3000};
    //int iterations [1] = {3000};
    double l_one_diff [11] = {0};
    //double l_one_diff [1] = {0};
    std::vector<step_log> temp_log;

    // One experiment for every presicion.
    for (int i = 0; i < 11; i++) {
        // Run greedy.
        link_rec.greedy(0.85, presicions[i], iterations[i]);
        // Get log_vec.
        temp_log = link_rec.get_log_vec();
        // Calculate precision of predictions. L1 Norm.
        for (int j = 0; j < log_s; j++) {
            l_one_diff[i] += std::fabs(temp_log[j].red_pagerank - temp_log[j].red_pagerank_prediction);
        }
    }
    // Write results in a text file.
    std::ofstream log_file("out_precision_greedy.txt");
    log_file << "eps\tL1_Diff\n";
    // For each precision.
    for (int i = 0; i < 11; i++) {
        log_file << presicions[i] << "\t" << l_one_diff[i] << std::endl;
    }
}

int main(int argc, char **argv) {
    // Init arguments.
    algorithm_mode algo_mode;
    int n_source = 1;
    int n_target = 1;

    if (!get_options(argc, argv, algo_mode, n_source, n_target)) return 1;

    // Init graph and algorithms.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    Edge_addition link_rec(g, algs, n_source, n_target);

    // Precision testing.
    //check_precision_effect(link_rec);
    
    switch (algo_mode)
    {
    case algorithm_mode::GREEDY :
        link_rec.greedy();
        link_rec.save_logs("greedy");
        break;
    case algorithm_mode::FAST_GREEDY :
        link_rec.fast_greedy();
        link_rec.save_logs("fast_greedy");
        break;
    default:
        std::cout << "Not supported yet\n";
        return 1;
    }
    
}