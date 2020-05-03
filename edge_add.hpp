#ifndef _EDGE_ADD_HPP
#define _EDGE_ADD_HPP

#include "graph.hpp"
#include "pagerank.hpp"

struct step_log {
    double red_pagerank;
    double red_pagerank_prediction;
    double red_pagerank_generalized_prediction;
};

struct edge {
    int source;
    int destination;
};

enum class algorithm_mode {GREEDY, FAST_GREEDY, APPROX, FAST_APPROX, RANDOM, RAND_SRC, ONE_TO_ALL_G,
    ONE_TO_ALL_FG, ONE_TO_ALL_R, SRC_ST, TAR_ST};
enum class s_select_criterion {};
enum class t_select_criterion {};

class Edge_addition {
    public:
        Edge_addition(graph &g, pagerank_algorithms &algs, int n_source = 10, int n_target = 100);
        void greedy_per_one(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void fast_greedy_per_one(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void fast_greedy_all(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void random_edges(int exp, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void random_sources_per_one(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void random_sources_all(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Every edge I calculate again.
        void one_to_all_greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // All conections are Calculated in the beginning.
        void one_to_all_fast_greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void one_to_all_random(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void source_stats(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void targets_stats(const double C=0.85, const double eps=1e-4, const int max_iter=100);
    
    private:
        // Get n best source nodes.
        std::vector<int> get_best_source_nodes(int n = 10, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Returns n_target or max number of of possible edges to add to a source node if n_target >.
        int get_valide_no_edges(int s_node);
        // Get best n target nodes.
        std::vector<int> get_best_target_nodes(int s_node, int no_targets);
        // Save vectors.
        void save_vector(std::string file_name, pagerank_v s_vector);
        void save_vector(std::string file_name, std::vector<int> s_vector);
        void save_vector(std::string file_name, std::vector<double> s_vector);
        void save_vector(std::string file_name, std::vector<edge> s_vector);
        void save_vector(std::string file_name, std::vector<step_log> s_vector);
        // Return values of Objective function.
        pagerank_v get_objective_val(int s_node, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        double get_generalized_objective_val(double init_red_pagerank, double init_source_pagerank, pagerank_v init_red_abs_prob, pagerank_v init_src_abs_prob,
            std::vector<int> init_src_nei, std::vector<edge> new_edges);

        graph &g;
        pagerank_algorithms &algs;

        double jump_prob = 0.15;
        int n_source, n_target; // Number of source, target nodes.

        std::vector<int> source_nodes;
};

#endif /* _EDGE_ADD_HPP */
