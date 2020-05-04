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

enum class algorithm_mode {RND_E, RND_ST, RND_S_GRD_T, GRD_S_RND_T, GRD_ST};
// CRITERION ONE: keep 2* best by pagerank, keep 1* best by blue abs prob.
// CRITERION TWO: 
enum class source_criterion {RANDOM, CRITERION_ONE};
enum class target_criterion {RANDOM, PREDICTION, GENERILIZED_PREDICTION};
enum class edge_criterion {RANDOM, CRITERION_ONE};
enum class prediction_kind {NONE, STEP, GENERILIZED};
enum class log_kind {NONE, PER_EDGE, PER_SOURCE, ALL_EDGES};

class Edge_addition {
    public:
        Edge_addition(graph &g, pagerank_algorithms &algs, int n_source = 10, int n_target = 100);
        void greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void greedy_per_source(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void greedy_all_sources(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void add_edges(int exp, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void add_edges_per_one(int exp, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void add_edges_all(int exp, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Every edge I calculate again.
        void one_to_all_greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // All conections are Calculated in the beginning.
        void one_to_all_fast_greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void one_to_all_random(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void source_stats(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void targets_stats(const double C=0.85, const double eps=1e-4, const int max_iter=100);
    
    private:
        // Get source nodes.
        std::vector<int> get_source_nodes(int n = 10, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        std::vector<int> get_source_nodes_random(int n = 10, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        std::vector<int> get_source_nodes_one(int n = 10, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Get target nodes.
        std::vector<int> get_target_nodes(int s_node, int no_targets);
        std::vector<int> get_target_nodes_random(int s_node, int no_targets);
        std::vector<int> get_target_nodes_pred(int s_node, int no_targets);
        // Get random edges.
        std::vector<edge> get_edges(int no_edges);
        std::vector<edge> get_edges_random(int no_edges);
        std::vector<edge> get_edges_one(int no_edges);
        // Remove new edges.
        void remove_new_edges(std::vector<edge> &new_edges);
        // std::vector<int> get_target_nodes_gen_pred(int s_node, int no_targets); Needs approximation it can't work greedy!!!
        // Returns n_target or max number of of possible edges to add to a source node if n_target >.
        int get_valide_no_edges(int s_node);
        // Save vectors.
        void save_vector(std::string file_name, pagerank_v &s_vector);
        void save_vector(std::string file_name, std::vector<int> &s_vector);
        void save_vector(std::string file_name, std::vector<double> &s_vector);
        void save_vector(std::string file_name, std::vector<edge> &s_vector);
        void save_vector(std::string file_name, std::vector<step_log> &s_vector);
        // Add log point to log vector.
        void add_log_point(std::vector<step_log> &log_vec, double red_pagerank = 0, double prediction = 0, double gen_prediction = 0);

        // Return values of Objective function.
        pagerank_v get_objective_val(int s_node, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        double get_gen_objective_val(double init_red_pagerank, double init_source_pagerank, pagerank_v init_red_abs_prob, pagerank_v init_src_abs_prob,
            std::vector<int> init_src_nei, std::vector<edge> new_edges);

        graph &g;
        pagerank_algorithms &algs;

        double jump_prob = 0.15;
        int n_source, n_target, n_edges; // Number of source, target nodes, edges.
        source_criterion s_criterion;
        target_criterion t_criterion;
        edge_criterion e_criterion;
        prediction_kind p_kind = prediction_kind::NONE;
        log_kind l_kind = log_kind::NONE;
};

#endif /* _EDGE_ADD_HPP */
