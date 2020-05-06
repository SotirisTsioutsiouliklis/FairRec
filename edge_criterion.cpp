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

int main () {
    // Declare variables.
    // algorithm_mode algo_mode;
    source_criterion s_criterion;
    target_criterion t_criterion;
    edge_criterion e_criterion;
    log_kind l_kind;
    int n_source = 10;
    int n_target = 30;
    int n_edges = 300;

    // Init graph and algorithms.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    Edge_addition link_rec(g, algs, n_source, n_target, n_edges);

    /** Run source heuristic with criterion one for source selection
     * greedy target selection, all log kind.
    */
    l_kind = log_kind::ALL_EDGES;
    //s_criterion = source_criterion::CRITERION_ONE;
    //t_criterion = target_criterion::STEP_PREDICTION;
    link_rec.set_criterion(l_kind);
    //link_rec.set_criterion(s_criterion);
    //link_rec.set_criterion(t_criterion);
    //link_rec.source_heuristic();
    
    /** Run edge heuristic with criterion for edge selection
     * random, all log kind.
    */
    s_criterion = source_criterion::RANDOM;
    t_criterion = target_criterion::RANDOM;
    e_criterion = edge_criterion::RANDOM;
    link_rec.set_criterion(s_criterion);
    link_rec.set_criterion(t_criterion);
    link_rec.set_criterion(e_criterion);
    link_rec.edge_heuristic();
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

}