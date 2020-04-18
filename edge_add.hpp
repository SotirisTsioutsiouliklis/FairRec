#ifndef _EDGE_ADD_HPP
#define _EDGE_ADD_HPP

#include "graph.hpp"
#include "pagerank.hpp"

struct step_log {
    double red_pagerank;
    double red_pagerank_prediction;
};

struct edge {
    int source;
    int destination;
};

enum class algorithm_mode {GREEDY, FAST_GREEDY, APPROX, FAST_APPROX};

class Edge_addition {
    public:
        Edge_addition(graph &g, pagerank_algorithms &algs, int n_source = 10, int n_target = 100);
        void greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void fast_greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void random_edges(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        //void approx();
        //void fast_approx();
        void save_logs(std::string algo_name);
        void save_src_logs(std::string algo_name, int src_node, std::vector<step_log> vec);
        int get_log_size();
        std::vector<step_log> get_log_vec();
    
    private:
        // Get n best source nodes.
        pagerank_v get_best_source_nodes(int n = 10, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Return values of Objective function.
        pagerank_v get_objective_val(int s_node, const double C=0.85, const double eps=1e-4, const int max_iter=100);

        graph &g;
        pagerank_algorithms &algs;
        std::vector<step_log> log_vec;

        double jump_prob = 0.15;
        int n_source, n_target; // Number of source, target nodes.
};

#endif /* _EDGE_ADD_HPP */
