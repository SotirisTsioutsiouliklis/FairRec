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

    //bool operator == (const edge &other) {
      //  return (source = other.source && destination == other.destination);
    //}
};

/**
 * Greedy: Calculates criterion after every edge addition.
 * Approximation one: Calculates criterion one time in the beginning. 
 */
enum class algorithm_mode {GREEDY, APPROX_ONE};
/**
 * Criterion one: Get 2*n best by pagerank. Keep those with the 
 *  biggest blue absorbing probability.
 */
enum class source_criterion {RANDOM, CRITERION_ONE};
/**
 * Step prediction: Calculate the maximum gain from the formula
 *  of the theory for only one edge addition.
 */
enum class target_criterion {RANDOM, STEP_PREDICTION};
/**
 * Criterion one - two: Create a score for each edge which is source pagerank
 *  destination red absorbing probability. Select the one with the
 *  greater score.
 * Criterion one: Product of pagerank source with target's red absorbing
 *  probability
 * Criterion one: Sum of pagerank source with target's red absorbing
 *  probability
 */ 
enum class edge_criterion {RANDOM, PRODUCT, SUM, FORMULA};
enum class prediction_kind {NONE, STEP, GENERILIZED};
enum class log_kind {NONE, PER_EDGE, PER_SOURCE, ALL_EDGES};

class Edge_addition {
    public:
        // Constractor.   
        Edge_addition(graph &g, pagerank_algorithms &algs, int n_source = 10, int n_target = 100, int n_edges = 1000);
        // Source heuristics.
        void source_heuristic(const double C=0.85, const double eps=1e-4, const int max_iter=100);   
        // Edge heuristics.
        void edge_heuristic(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Setters.
        void set_criterion(source_criterion s_crit);
        void set_criterion(target_criterion t_crit);
        void set_criterion(edge_criterion e_crit);
        void set_criterion(prediction_kind p_crit);
        void set_criterion(log_kind l_crit);
        void set_criterion(algorithm_mode a_md);
    private:
        // Source heuristics.
        void source_heuristic_per_one(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void source_heuristic_per_source(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void source_heuristic_all_sources(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Edge heuristics.
        void edge_heuristic_per_one(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void edge_heuristic_all(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void edge_heuristic_all_greedy(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Get source nodes.
        std::vector<int> get_source_nodes(int n = 10, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        std::vector<int> get_source_nodes_random(int n = 10);
        std::vector<int> get_source_nodes_one(int n = 10, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Get target nodes.
        pagerank_v get_target_nodes(int s_node, int no_targets);
        pagerank_v get_target_nodes_random(int s_node, int no_targets);
        pagerank_v get_target_nodes_pred(int s_node, int no_targets);
        // Get random edges.
        std::vector<edge> get_edges(int no_edges, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        std::vector<edge> get_edges_random(int no_edges);
        std::vector<edge> get_edges_one(int no_edges, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        std::vector<edge> get_edges_two(int no_edges, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        std::vector<edge> get_edges_three(int no_edges, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        // Remove new edges.
        void remove_new_edges(std::vector<edge> &new_edges);
        // Get valide number of edges.
        int get_valide_no_edges(int s_node);
        // Save various vectors.
        void save_vector(std::string file_name, pagerank_v &s_vector);
        void save_vector(std::string file_name, std::vector<int> &s_vector);
        void save_vector(std::string file_name, std::vector<double> &s_vector);
        void save_vector(std::string file_name, std::vector<edge> &s_vector);
        void save_vector(std::string file_name, std::vector<step_log> &s_vector);
        // Add log point to log vector.
        void add_log_point(std::vector<step_log> &log_vec, double red_pagerank = 0, double prediction = 0, double gen_prediction = 0);
        // Return values of Objective function.
        pagerank_v get_objective_val(int s_node, const double C=0.85, const double eps=1e-4, const int max_iter=100);
        double get_gen_objective_val(int s_node, std::vector<edge> new_edges,  const double C=0.85);

        graph &g;
        pagerank_algorithms &algs;
        int n_source, n_target, n_edges; // Number of source, target nodes, edges.
        algorithm_mode a_mode;
        source_criterion s_criterion;
        target_criterion t_criterion;
        edge_criterion e_criterion;
        prediction_kind p_kind;
        log_kind l_kind;
};

#endif /* _EDGE_ADD_HPP */
