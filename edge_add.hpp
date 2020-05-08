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
    int s_outd;
    double s_score_1, s_score_2, t_score, e_score;

    //bool operator == (const edge &other) {
      //  return (source = other.source && destination == other.destination);
    //}
};

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> parent of 10b851b... fix branches conflicts
enum class algorithm_mode {GREEDY, FAST_GREEDY, APPROX, FAST_APPROX, RANDOM, RAND_SRC, ONE_TO_ALL_G,
    ONE_TO_ALL_FG, ONE_TO_ALL_R, SRC_ST, TAR_ST};
enum class s_select_criterion {};
enum class t_select_criterion {};
=======
enum class algorithm_mode {GREEDY, FAST_GREEDY, APPROX, FAST_APPROX, RANDOM, RAND_SRC, ONE_TO_ALL_G, ONE_TO_ALL_FG, ONE_TO_ALL_R};
>>>>>>> parent of 90cc69f... sources and targets in progress

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
<<<<<<< HEAD
        void source_stats(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        void targets_stats(const double C=0.85, const double eps=1e-4, const int max_iter=100);
=======
        void source_and_targets(const double C=0.85, const double eps=1e-4, const int max_iter=100);
        //void approx();
        //void fast_approx();
        void save_logs(std::string algo_name, std::vector<step_log> log_vec);
        void save_logs_per_node(std::string algo_name, int src_node, std::vector<step_log> vec);
>>>>>>> parent of 90cc69f... sources and targets in progress
    
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
<<<<<<< HEAD
=======
        // Save Source Nodes.
        void save_source_nodes(std::string algo_name, std::vector<int> sources);
        void save_target_nodes(std::string algo_name, std::vector<int> sources);
        // Save impact.
        void save_impact(std::vector<double> impact_v);
>>>>>>> parent of 90cc69f... sources and targets in progress

        graph &g;
        pagerank_algorithms &algs;

        double jump_prob = 0.15;
        int n_source, n_target; // Number of source, target nodes.
=======
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
enum class edge_criterion {RANDOM, PRODUCT, SUM, FORMULA, WEIGHTED_PRODUCT};
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
        std::vector<edge> get_edges_four(int no_edges, const double C=0.85, const double eps=1e-4, const int max_iter=100);
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
>>>>>>> refactoring
};

#endif /* _EDGE_ADD_HPP */
