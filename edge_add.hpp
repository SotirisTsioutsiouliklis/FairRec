#ifndef _EDGE_ADD_HPP
#define _EDGE_ADD_HPP

#include "graph.hpp"
#include "pagerank.hpp"

struct step_log {
    double red_pagerank;
    double red_pagerank_prediction;
};

enum class algorithm_mode {GREEDY, FAST_GREEDY, APPROX, FAST_APPROX};

class Edge_addition {
    public:
        Edge_addition(graph &g, pagerank_algorithms &algs, int n_source = 10, int n_target = 100);
        void greedy();
        void fast_greedy();
        //void approx();
        //void fast_approx();
    
    private:
        // n best nodes.
        void get_best_source_nodes(int n = 10);
        // k best nodes for the u node. Edge (u, k_i).
        pagerank_v get_best_target_nodes(int u, int k = 100);
        // Return Value of Objective function.
        pagerank_v get_objective_val(int s_node);
        double get_prediction(pagerank_t node);

        graph &g;
        pagerank_algorithms &algs;
        int n_source, n_target; // Number of source, target nodes.
        pagerank_v rank_vector, red_pagerank, objective_val, top_source_nodes, top_target_nodes;
        std::vector<step_log> log_vec;
        double red_pagerank;
        pagerank_s target_node;
        std::vector<int> neighbors;
};

#endif /* _EDGE_ADD_HPP */
