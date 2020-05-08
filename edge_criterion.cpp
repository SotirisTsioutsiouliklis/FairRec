/**
 * This program has purpose to compare edge selection techniques. 
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
#include "edge_add.hpp"
#include <chrono>
#include <omp.h> 

int main () {
    // Define number of threads to use.
    omp_set_num_threads(20);
    auto start =std::chrono::high_resolution_clock::now();
    srand(time(NULL));

    // Declare variables.
    algorithm_mode algo_mode;
    source_criterion s_criterion;
    target_criterion t_criterion;
    edge_criterion e_criterion;
    algorithm_mode a_mode;
    log_kind l_kind;
    int n_source = 30;
    int n_target = 100;
    int n_edges = 300;

    // Init graph and algorithms.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    Edge_addition link_rec(g, algs, n_source, n_target, n_edges);

    // Init log criterion.
    l_kind = log_kind::ALL_EDGES;
    link_rec.set_criterion(l_kind);
    // Init algorithmcriterion.
    algo_mode = algorithm_mode::APPROX_ONE;
    link_rec.set_criterion(algo_mode);
    /** Run edge heuristic with criterion for edge selection
     * random, all log kind.
    */
    s_criterion = source_criterion::RANDOM;
    t_criterion = target_criterion::RANDOM;
    e_criterion = edge_criterion::RANDOM;
    a_mode = algorithm_mode::APPROX_ONE;
    link_rec.set_criterion(s_criterion);
    link_rec.set_criterion(t_criterion);
    link_rec.set_criterion(e_criterion);
    link_rec.set_criterion(a_mode);
    //link_rec.edge_heuristic();
    /** Run edge heuristic with criterion one for edge selection
     * , all log kind.
    */
    e_criterion = edge_criterion::PRODUCT;
    link_rec.set_criterion(e_criterion);
    link_rec.edge_heuristic();
    /** Run edge heuristic with criterion two for edge selection
     * , all log kind.
    */
    e_criterion = edge_criterion::SUM;
    link_rec.set_criterion(e_criterion);
    link_rec.edge_heuristic();
    /** Run edge heuristic with criterion twhree for edge selection
     * , all log kind.
    */
    e_criterion = edge_criterion::FORMULA;
    link_rec.set_criterion(e_criterion);
    link_rec.edge_heuristic();
    /** Run edge heuristic with criterion two for edge selection
     * , all log kind.
    */
    e_criterion = edge_criterion::WEIGHTED_PRODUCT;
    link_rec.set_criterion(e_criterion);
    link_rec.edge_heuristic();
    /** Run edge heuristic with criterion tfour for edge selection
     * , all log kind.
    */
    //a_mode = algorithm_mode::GREEDY;
    //link_rec.set_criterion(a_mode);
    //e_criterion = edge_criterion::FORMULA;
    //link_rec.set_criterion(e_criterion);
    //link_rec.edge_heuristic();

    auto stop =std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Total time: " << duration.count() << " microseconds\n"; 
}