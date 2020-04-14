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
        void save_logs();
    
    private:
        // Get n best source nodes.
        pagerank_v get_best_source_nodes(int n = 10);
        // Return values of Objective function.
        pagerank_v get_objective_val(int s_node);
        double get_prediction(pagerank_t node);

        graph &g;
        pagerank_algorithms &algs;
        std::vector<step_log> log_vec;

        double jump_prob = 0.15;
        int n_source, n_target; // Number of source, target nodes.
};

#endif /* _EDGE_ADD_HPP */
