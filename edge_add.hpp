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

enum class algorithm_mode {GREEDY, FAST_GREEDY, APPROX, FAST_APPROX, RANDOM, RAND_SRC};

class Edge_addition {
    public:
        Edge_addition(graph &g, pagerank_algorithms &algs, int n_source = 10, int n_target = 100);
        void greedy_per_one(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void greedy_all(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void fast_greedy_per_one(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void fast_greedy_all(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void random_edges(int exp, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void random_sources_per_one(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void random_sources_all(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        //void approx();
        //void fast_approx();
        void save_logs(std::string algo_name, std::vector<step_log> log_vec);
        void save_logs_per_node(std::string algo_name, int src_node, std::vector<step_log> vec);
    
    private:
        // Get n best source nodes.
        pagerank_v get_best_source_nodes(int n = 10, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Return values of Objective function.
        pagerank_v get_objective_val(int s_node, const double C=0.85, const double eps=1e-4, const int max_iter=100);

        graph &g;
        pagerank_algorithms &algs;
        //std::vector<step_log> log_vec;

        double jump_prob = 0.15;
        int n_source, n_target; // Number of source, target nodes.
};

#endif /* _EDGE_ADD_HPP */
