#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "edge_add.hpp"
#include <ctime>
#include <cstdlib>

// Class constructor.
Edge_addition::Edge_addition(graph &g, pagerank_algorithms &algs, int n_source, int n_target) : g(g), algs(algs),
    n_source(n_source), n_target(n_target) {
        // Nothing to do here.
}

void Edge_addition::greedy_per_one(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, source_nodes;
    std::vector<int> neighbors;
    std::vector<edge> new_edges;
    std::vector<step_log> log_vec;
    pagerank_s target_node;
    edge new_edge;
    step_log log_point;
    double red_pagerank;
    int s_out_degree, src_node, max_edges;
    int wanted_target = n_target;

    // Get number of nodes.
    const int nnodes = g.get_num_nodes();

    // Get source nodes.
    source_nodes = get_best_source_nodes(n_source, C, eps, max_iter);

    // For each Source nodes.
    for (int s_node = 0; s_node < n_source; s_node++) {
        // Zeroing new edges list.
        new_edges.clear();
        // Zeroing log vector.
        log_vec.clear();
        // Get pagerank.
        rank_vector = algs.get_pagerank(C, eps, max_iter);
        // Get Red pagerank.
        red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
        // Renew log point.
        log_point.red_pagerank = red_pagerank;
        std::cout << "Red pagerank: " << red_pagerank << std::endl;
        log_point.red_pagerank_prediction = red_pagerank;
        // Store to log vector.
        log_vec.push_back(log_point);
        // Get source node id.
        src_node = source_nodes[s_node].node_id;
        // Print Source node.
        std::cout << "---------------------------------------\n";
        std::cout << "Source node: " << src_node << std::endl;
        // Find valide number of new edges.
        max_edges = (g.get_num_nodes() - 1) - g.get_out_degree(src_node);
        n_target = wanted_target;
        if (n_target > max_edges) {
            std::cout << "too many nodes. I'll do my best" << std::endl;
            n_target = max_edges;
        }
        std::cout << "ntarget: " << n_target << "\n";
        // Find n_target nodes to connect.
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get objective values for all nodes.
            objective_val = get_objective_val(src_node, C, eps, max_iter);
            // Get out degree for source node.
            s_out_degree = g.get_out_degree(src_node);
            // Remove neighbors of source node from "finding the
            // best" procedure. Do that by setting their objective value to -2.
            neighbors = g.get_out_neighbors(src_node);
            for (int nei = 0; nei < s_out_degree; nei++) { 
                objective_val[neighbors[nei]].pagerank = -2;
            }
            // Remove self from "finding the best procedure".
            // Do that by setting its objective value to -2.
            objective_val[src_node].pagerank = -2;
            // Find best edge to add.
            target_node.node_id = -2;
            target_node.pagerank = -2;
            // Search all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is better, Renew node information.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add best node to source node.
            g.add_edge(src_node, target_node.node_id);
            new_edge.source = src_node;
            new_edge.destination = target_node.node_id;
            new_edges.push_back(new_edge);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            log_point.red_pagerank = red_pagerank;
            log_point.red_pagerank_prediction = target_node.pagerank;
            // Store to log vector.
            log_vec.push_back(log_point);
        }
        //Remove edges from the graph.
        for (std::vector<edge>::iterator i = new_edges.begin(); i < new_edges.end(); i++) {
            g.remove_edge(i->source, i->destination);
        }
        // Save per node logs.
        save_logs_per_node("greedy_per_one", src_node, log_vec);
    }
    
}

void Edge_addition::greedy_per_one_generalized(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, source_nodes;
    std::vector<int> neighbors;
    std::vector<edge> new_edges;
    std::vector<step_log> log_vec;
    pagerank_s target_node;
    edge new_edge;
    step_log log_point;
    double red_pagerank;
    int s_out_degree, src_node, max_edges;
    int wanted_target = n_target;

    // Get number of nodes.
    const int nnodes = g.get_num_nodes();

    // Get source nodes.
    source_nodes = get_best_source_nodes(n_source, C, eps, max_iter);

    // For each Source nodes.
    for (int s_node = 0; s_node < n_source; s_node++) {
        // Zeroing new edges list.
        new_edges.clear();
        // Zeroing log vector.
        log_vec.clear();
        // Get pagerank.
        rank_vector = algs.get_pagerank(C, eps, max_iter);
        // Get Red pagerank.
        red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
        // Renew log point.
        log_point.red_pagerank = red_pagerank;
        std::cout << "Red pagerank: " << red_pagerank << std::endl;
        log_point.red_pagerank_prediction = red_pagerank;
        // Store to log vector.
        log_vec.push_back(log_point);
        // Get source node id.
        src_node = source_nodes[s_node].node_id;
        // Print Source node.
        std::cout << "---------------------------------------\n";
        std::cout << "Source node: " << src_node << std::endl;
        // Find valide number of new edges.
        max_edges = (g.get_num_nodes() - 1) - g.get_out_degree(src_node);
        n_target = wanted_target;
        if (n_target > max_edges) {
            std::cout << "too many nodes. I'll do my best" << std::endl;
            n_target = max_edges;
        }
        std::cout << "ntarget: " << n_target << "\n";
        // Find n_target nodes to connect.
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get objective values for all nodes.
            objective_val = get_objective_val(src_node, C, eps, max_iter);
            // Get out degree for source node.
            s_out_degree = g.get_out_degree(src_node);
            // Remove neighbors of source node from "finding the
            // best" procedure. Do that by setting their objective value to -2.
            neighbors = g.get_out_neighbors(src_node);
            for (int nei = 0; nei < s_out_degree; nei++) { 
                objective_val[neighbors[nei]].pagerank = -2;
            }
            // Remove self from "finding the best procedure".
            // Do that by setting its objective value to -2.
            objective_val[src_node].pagerank = -2;
            // Find best edge to add.
            target_node.node_id = -2;
            target_node.pagerank = -2;
            // Search all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is better, Renew node information.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add best node to source node.
            g.add_edge(src_node, target_node.node_id);
            new_edge.source = src_node;
            new_edge.destination = target_node.node_id;
            new_edges.push_back(new_edge);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            log_point.red_pagerank = red_pagerank;
            log_point.red_pagerank_prediction = target_node.pagerank;
            // Store to log vector.
            log_vec.push_back(log_point);
        }
        //Remove edges from the graph.
        for (std::vector<edge>::iterator i = new_edges.begin(); i < new_edges.end(); i++) {
            g.remove_edge(i->source, i->destination);
        }
        // Save per node logs.
        save_logs_per_node("greedy_per_one", src_node, log_vec);
    }
    
}

void Edge_addition::greedy_all(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, source_nodes;
    std::vector<int> neighbors;
    std::vector<step_log> log_vec;
    pagerank_s target_node;
    step_log log_point;
    double red_pagerank;
    int s_out_degree, src_node, max_edges;
    int wanted_target = n_target;

    // Get number of nodes.
    const int nnodes = g.get_num_nodes();

    // Get source nodes.
    source_nodes = get_best_source_nodes(n_source, C, eps, max_iter);
    // Get pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Get Red pagerank.
    red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Renew log point.
    log_point.red_pagerank = red_pagerank;
    std::cout << "Red pagerank: " << red_pagerank << std::endl;
    log_point.red_pagerank_prediction = red_pagerank;
    // Store to log vector.
    log_vec.push_back(log_point);

    // For each Source nodes.
    for (int s_node = 0; s_node < n_source; s_node++) {
        // Get source node id.
        src_node = source_nodes[s_node].node_id;
        // Print Source node.
        std::cout << "---------------------------------------\n";
        std::cout << "Source node: " << src_node << std::endl;
        // Find valide number of new edges.
        max_edges = (g.get_num_nodes() - 1) - g.get_out_degree(src_node);
        n_target = wanted_target;
        if (n_target > max_edges) {
            std::cout << "too many nodes. I'll do my best" << std::endl;
            n_target = max_edges;
        }
        std::cout << "ntarget: " << n_target << "\n";
        // Find n_target nodes to connect.
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get objective values for all nodes.
            objective_val = get_objective_val(src_node, C, eps, max_iter);
            // Get out degree for source node.
            s_out_degree = g.get_out_degree(src_node);
            // Remove neighbors of source node from "finding the
            // best" procedure. Do that by setting their objective value to -2.
            neighbors = g.get_out_neighbors(src_node);
            for (int nei = 0; nei < s_out_degree; nei++) { 
                objective_val[neighbors[nei]].pagerank = -2;
            }
            // Remove self from "finding the best procedure".
            // Do that by setting its objective value to -2.
            objective_val[src_node].pagerank = -2;
            // Find best edge to add.
            target_node.node_id = -2;
            target_node.pagerank = -2;
            // Search all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is better, Renew node information.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add best node to source node.
            g.add_edge(src_node, target_node.node_id);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            log_point.red_pagerank = red_pagerank;
            log_point.red_pagerank_prediction = target_node.pagerank;
            // Store to log vector.
            log_vec.push_back(log_point);
        }
    }
    // Save log vector.
    save_logs("greedy_all", log_vec);
}

void Edge_addition::fast_greedy_per_one(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, source_nodes;
    std::vector<int> neighbors;
    std::vector<edge> new_edges;
    std::vector<step_log> log_vec;
    pagerank_s target_node;
    edge new_edge;
    step_log log_point;
    double red_pagerank;
    int s_out_degree, src_node, max_edges;
    int wanted_target = n_target;

    // Get number of nodes.
    const int nnodes = g.get_num_nodes();

    // Get source nodes.
    source_nodes = get_best_source_nodes(n_source, C, eps, max_iter);

    // For each Source nodes.
    for (int s_node = 0; s_node < n_source; s_node++) {
        // Zeroing new edges list.
        new_edges.clear();
        // Zeroing log vector.
        log_vec.clear();
        // Get pagerank.
        rank_vector = algs.get_pagerank(C, eps, max_iter);
        // Get Red pagerank.
        red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
        // Renew log point.
        log_point.red_pagerank = red_pagerank;
        log_point.red_pagerank_prediction = red_pagerank;
        // Store to log vector.
        log_vec.push_back(log_point);
        // Get source node id.
        src_node = source_nodes[s_node].node_id;
        // Print Source node.
        std::cout << "---------------------------------------\n";
        std::cout << "Source node: " << src_node << std::endl;
        // Find valide number of new edges.
        max_edges = (g.get_num_nodes() - 1) - g.get_out_degree(src_node);
        n_target = wanted_target;
        if (n_target > max_edges) {
            std::cout << "too many nodes. I'll do my best" << std::endl;
            n_target = max_edges;
        }
        std::cout << "ntarget: " << n_target << "\n";
        // Get objective values for all nodes.
        objective_val = get_objective_val(src_node, C, eps, max_iter);
        // Find n_target nodes to connect.
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get out degree for source node.
            s_out_degree = g.get_out_degree(src_node);
            // Remove neighbors of source node from "finding the
            // best" procedure. Do that by setting their objective value to -2.
            neighbors = g.get_out_neighbors(src_node);
            for (int nei = 0; nei < s_out_degree; nei++) { 
                objective_val[neighbors[nei]].pagerank = -2;
            }
            // Remove self from "finding the best procedure".
            // Do that by setting its objective value to -2.
            objective_val[src_node].pagerank = -2;
            // Find best edge to add.
            target_node.node_id = -2;
            target_node.pagerank = -2;
            // Search all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is better, Renew node information.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add best node to source node.
            g.add_edge(src_node, target_node.node_id);
            new_edge.source = src_node;
            new_edge.destination = target_node.node_id;
            new_edges.push_back(new_edge);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            log_point.red_pagerank = red_pagerank;
            log_point.red_pagerank_prediction = target_node.pagerank;
            // Store to log vector.
            log_vec.push_back(log_point);
        }
        //Remove edges from the graph.
        for (std::vector<edge>::iterator i = new_edges.begin(); i < new_edges.end(); i++) {
            g.remove_edge(i->source, i->destination);
        }
        // Save per node logs.
        save_logs_per_node("fast_greedy_per_one", src_node, log_vec);
    }
}

void Edge_addition::fast_greedy_all(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, objective_val_init, rank_vector, source_nodes;
    std::vector<int> neighbors;
    std::vector<edge> new_edges;
    std::vector<step_log> log_vec;
    pagerank_s target_node;
    step_log log_point;
    double red_pagerank;
    int s_out_degree, src_node, max_edges;
    int wanted_target = n_target;

    // Get number of nodes.
    const int nnodes = g.get_num_nodes();

    // Get source nodes.
    source_nodes = get_best_source_nodes(n_source, C, eps, max_iter);
    // Get pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Get Red pagerank.
    red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Renew log point.
    log_point.red_pagerank = red_pagerank;
    log_point.red_pagerank_prediction = red_pagerank;
    // Store to log vector.
    log_vec.push_back(log_point);

    // For each Source nodes.
    for (int s_node = 0; s_node < n_source; s_node++) {
        // Zeroing new edges list.
        new_edges.clear();
        // Get source node id.
        src_node = source_nodes[s_node].node_id;
        // Get objective values for all nodes.
        objective_val_init = get_objective_val(src_node, C, eps, max_iter);
        // Print Source node.
        std::cout << "---------------------------------------\n";
        std::cout << "Source node: " << src_node << std::endl;
        // Find valide number of new edges.
        max_edges = (g.get_num_nodes() - 1) - g.get_out_degree(src_node);
        n_target = wanted_target;
        if (n_target > max_edges) {
            std::cout << "too many nodes. I'll do my best" << std::endl;
            n_target = max_edges;
        }
        std::cout << "ntarget: " << n_target << "\n";
        // Find n_target nodes to connect.
        objective_val = objective_val_init;
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get out degree for source node.
            s_out_degree = g.get_out_degree(src_node);
            // Remove neighbors of source node from "finding the
            // best" procedure. Do that by setting their objective value to -2.
            neighbors = g.get_out_neighbors(src_node);
            for (int nei = 0; nei < s_out_degree; nei++) { 
                objective_val[neighbors[nei]].pagerank = -2;
            }
            // Remove self from "finding the best procedure".
            // Do that by setting its objective value to -2.
            objective_val[src_node].pagerank = -2;
            // Find best edge to add.
            target_node.node_id = -2;
            target_node.pagerank = -2;
            // Search all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is better, Renew node information.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add best node to source node.
            g.add_edge(src_node, target_node.node_id);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            log_point.red_pagerank = red_pagerank;
            log_point.red_pagerank_prediction = target_node.pagerank;
            // Store to log vector.
            log_vec.push_back(log_point);
        }
    }
    // Save log vector.
    save_logs("fast_greedy_all", log_vec);
}

void Edge_addition::random_edges(int exp, const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v rank_vector;
    std::vector<edge> new_edges;
    std::vector<int> neighbors, no_neighbors;
    std::vector<step_log> log_vec;
    edge new_edge;
    int n_of_edges = n_source * n_target;
    int nnodes = g. get_num_nodes();
    std::vector<int> all_nodes(nnodes);
    for (int i = 0; i < nnodes; i++) {
        all_nodes[i] = i;
    }

    // Resize log_vec.
    log_vec.resize(n_of_edges + 1);
    // Get pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    log_vec[0].red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Init seed.
    srand(time(NULL));
    // Find valide edges.
    for (int i = 0; i < n_of_edges; i++) {
        // Test print.
        std::cout << "Edge: " << i << std::endl;
        // Get random from 0 to nnodes - 1.
        new_edge.source = (rand() % nnodes);
        neighbors = g.get_out_neighbors(new_edge.source);
        // Sort vectors for difference.
        std::sort(all_nodes.begin(), all_nodes.end());
        std::sort(neighbors.begin(), neighbors.end());
        //std::vector<int> difference;
        std::set_difference(
            all_nodes.begin(), all_nodes.end(),
            neighbors.begin(), neighbors.end(),
            std::back_inserter( no_neighbors )
        );
        // Check if there are acceptable edges.
        if (no_neighbors.size() != 0) {
            // Get random target.
            new_edge.destination = no_neighbors[(rand() % no_neighbors.size())];
            std::cout << "edge: " << new_edge.source << " , " << new_edge.destination << std::endl;
            // Clear no_neighbors.
            no_neighbors.clear();
            // Add edge.
            g.add_edge(new_edge.source, new_edge.destination);
            // Add to new edges vector.
            new_edges.push_back(new_edge);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            log_vec[i + 1].red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
        } else {
            i--;
        }
    }
    // Save log vector.
    save_logs("random_edges_" + std::to_string(exp), log_vec);
    //Remove edges from the graph.
    for (std::vector<edge>::iterator i = new_edges.begin(); i < new_edges.end(); i++) {
        g.remove_edge(i->source, i->destination);
    }
}

void Edge_addition::random_sources_per_one(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector;
    std::vector<int> neighbors, source_nodes;
    std::vector<edge> new_edges;
    std::vector<step_log> log_vec;
    pagerank_s target_node;
    edge new_edge;
    step_log log_point;
    double red_pagerank;
    int s_out_degree, src_node, max_edges;
    int wanted_target = n_target;
    bool is_source;

    // Get number of nodes.
    const int nnodes = g.get_num_nodes();

    // Init seed.
    srand(time(NULL));
    // Get random source nodes.
    source_nodes.resize(n_source);
    for (int i = 0; i < n_source; i++) {
        do {
            // Get random int from 0 to nnodes - 1.
            src_node = (rand() % nnodes);
            // Check if already exists.
            if (std::find(source_nodes.begin(), source_nodes.end(), src_node) != source_nodes.end()) {
                is_source = true;
            } else {
                is_source = false;
            }
        } while (is_source);
        // Add node to sources.
        source_nodes[i] = src_node;
    }

    // For each Source nodes.
    for (int s_node = 0; s_node < n_source; s_node++) {
        // Zeroing new edges list.
        new_edges.clear();
        // Zeroing log vector.
        log_vec.clear();
        // Get pagerank.
        rank_vector = algs.get_pagerank(C, eps, max_iter);
        // Get Red pagerank.
        red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
        // Renew log point.
        log_point.red_pagerank = red_pagerank;
        log_point.red_pagerank_prediction = red_pagerank;
        // Store to log vector.
        log_vec.push_back(log_point);
        // Get source node id.
        src_node = source_nodes[s_node];
        // Print Source node.
        std::cout << "---------------------------------------\n";
        std::cout << "Source node: " << src_node << std::endl;
        // Find valide number of new edges.
        max_edges = (g.get_num_nodes() - 1) - g.get_out_degree(src_node);
        n_target = wanted_target;
        if (n_target > max_edges) {
            std::cout << "too many nodes. I'll do my best" << std::endl;
            n_target = max_edges;
        }
        std::cout << "ntarget: " << n_target << "\n";
        // Find n_target nodes to connect.
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get objective values for all nodes.
            objective_val = get_objective_val(src_node, C, eps, max_iter);
            // Get out degree for source node.
            s_out_degree = g.get_out_degree(src_node);
            // Remove neighbors of source node from "finding the
            // best" procedure. Do that by setting their objective value to -2.
            neighbors = g.get_out_neighbors(src_node);
            for (int nei = 0; nei < s_out_degree; nei++) { 
                objective_val[neighbors[nei]].pagerank = -2;
            }
            // Remove self from "finding the best procedure".
            // Do that by setting its objective value to -2.
            objective_val[src_node].pagerank = -2;
            // Find best edge to add.
            target_node.node_id = -2;
            target_node.pagerank = -2;
            // Search all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is better, Renew node information.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add best node to source node.
            g.add_edge(src_node, target_node.node_id);
            new_edge.source = src_node;
            new_edge.destination = target_node.node_id;
            new_edges.push_back(new_edge);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            log_point.red_pagerank = red_pagerank;
            log_point.red_pagerank_prediction = target_node.pagerank;
            // Store to log vector.
            log_vec.push_back(log_point);
        }
        //Remove edges from the graph.
        for (std::vector<edge>::iterator i = new_edges.begin(); i < new_edges.end(); i++) {
            g.remove_edge(i->source, i->destination);
        }
        // Save per node logs.
        save_logs_per_node("random_source_per_one", s_node, log_vec);
    }

}

void Edge_addition::random_sources_all(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector;
    std::vector<int> neighbors, source_nodes;
    std::vector<edge> new_edges;
    std::vector<step_log> log_vec;
    pagerank_s target_node;
    step_log log_point;
    double red_pagerank;
    int s_out_degree, src_node, max_edges;
    int wanted_target = n_target;
    bool is_source;

    // Get number of nodes.
    const int nnodes = g.get_num_nodes();

    // Init seed.
    srand(time(NULL));
    // Get random source nodes.
    source_nodes.resize(n_source);
    for (int i = 0; i < n_source; i++) {
        do {
            // Get random int from 0 to nnodes - 1.
            src_node = (rand() % nnodes);
            // Check if alwready exists.
            if (std::find(source_nodes.begin(), source_nodes.end(), src_node) != source_nodes.end()) {
                is_source = true;
            } else {
                is_source = false;
            }
        } while (is_source);
        // Add node to sources.
        source_nodes[i] = src_node;
    }
    // Get pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Get Red pagerank.
    red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Renew log point.
    log_point.red_pagerank = red_pagerank;
    log_point.red_pagerank_prediction = red_pagerank;
    // Store to log vector.
    log_vec.push_back(log_point);

    // For each Source nodes.
    for (int s_node = 0; s_node < n_source; s_node++) {
        // Zeroing new edges list.
        new_edges.clear();
        // Get source node id.
        src_node = source_nodes[s_node];
        // Print Source node.
        std::cout << "---------------------------------------\n";
        std::cout << "Source node: " << src_node << std::endl;
        // Find valide number of new edges.
        max_edges = (g.get_num_nodes() - 1) - g.get_out_degree(src_node);
        n_target = wanted_target;
        if (n_target > max_edges) {
            std::cout << "too many nodes. I'll do my best" << std::endl;
            n_target = max_edges;
        }
        std::cout << "ntarget: " << n_target << "\n";
        // Find n_target nodes to connect.
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get objective values for all nodes.
            objective_val = get_objective_val(src_node, C, eps, max_iter);
            // Get out degree for source node.
            s_out_degree = g.get_out_degree(src_node);
            // Remove neighbors of source node from "finding the
            // best" procedure. Do that by setting their objective value to -2.
            neighbors = g.get_out_neighbors(src_node);
            for (int nei = 0; nei < s_out_degree; nei++) { 
                objective_val[neighbors[nei]].pagerank = -2;
            }
            // Remove self from "finding the best procedure".
            // Do that by setting its objective value to -2.
            objective_val[src_node].pagerank = -2;
            // Find best edge to add.
            target_node.node_id = -2;
            target_node.pagerank = -2;
            // Search all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is better, Renew node information.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add best node to source node.
            g.add_edge(src_node, target_node.node_id);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            log_point.red_pagerank = red_pagerank;
            log_point.red_pagerank_prediction = target_node.pagerank;
            // Store to log vector.
            log_vec.push_back(log_point);
        }
    }
    // Save log vector.
    save_logs("random_source_all", log_vec);
}

pagerank_v Edge_addition::get_best_source_nodes(int n, const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v rank_vector, top_source_nodes;
    int src_node;

    // Get Pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Sort pagerank in descentding order.
    algs.sort_pagerank_vector(rank_vector);
    // Resize top_source_vector to 2*n. Keep top 2*n.
    top_source_nodes.resize(2*n);
    for (int i = 0; i < 2*n; i++) {
        top_source_nodes[i] = rank_vector[i];
    }
    // Get Blue personilized pagerank.
    rank_vector = algs.get_blue_abs_prob(C, eps, max_iter);
    // Change top source pagerank to Blue personilized.
    for (int i = 0; i < 2*n; i++) {
        src_node = top_source_nodes[i].node_id;
        top_source_nodes[i].pagerank = rank_vector[src_node].pagerank;
    }
    // Sort top nodes by Blue pagerank.
    algs.sort_pagerank_vector(top_source_nodes);
    // Keep n top source nodes by this ranking.
    top_source_nodes.resize(n);
    // Print best nodes in order.
    std::cout << "----------Best source nodes---------" << std::endl;
    for (int i = 0; i < n; i++) {
        std::cout << top_source_nodes[i].node_id << ", ";
    }
    std::cout << std::endl;

    return top_source_nodes;
}

pagerank_v Edge_addition::get_objective_val(int s_node, const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, red_absorbing_probs, source_absorbing_probs;
    std::vector<int> neighbors;
    double red_pagerank, nominator_const, denominator_const, obj_nominator, obj_denominator;
    int s_out_degree, neighbor;
    const int nnodes = g.get_num_nodes();
    objective_val.resize(nnodes);

    // Get source out degree.
    s_out_degree = g.get_out_degree(s_node);
    // Run pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Get red pagerank.
    red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Run absoring to Red.
    red_absorbing_probs = algs.get_red_abs_prob(C, eps, max_iter);
    // Run absorbing to source.
    source_absorbing_probs = algs.get_node_abs_prob(s_node, C, eps, max_iter);
    // Get source neighbors.
    neighbors = g.get_out_neighbors(s_node);
    // Get average Red pagerank of neighbors for nominator.
    nominator_const = 0;
    if (s_out_degree > 0) {
        for (int nei = 0; nei < s_out_degree; nei++) {
            neighbor = neighbors[nei];
            nominator_const += red_absorbing_probs[neighbor].pagerank;
        }
        nominator_const *= (1 / (float)s_out_degree);
    } else {
        for (int nei = 0; nei < nnodes; nei++) {
            neighbor = nei;
            nominator_const += red_absorbing_probs[neighbor].pagerank;
        }
        nominator_const *= (1 / (float)nnodes);
    }
    // Get average Source pagerank of neighbors for denominator.
    denominator_const = 0;
    if (s_out_degree > 0) {
        for (int nei = 0; nei < s_out_degree; nei++) {
            neighbor = neighbors[nei];
            denominator_const += source_absorbing_probs[neighbor].pagerank;
        }
        denominator_const *= (1 / (float)s_out_degree);
    } else {
        for (int nei = 0; nei < nnodes; nei++) {
            neighbor = nei;
            denominator_const += source_absorbing_probs[neighbor].pagerank;
        }
        denominator_const *= (1 / (float)nnodes);
    }
    // Calculate the Quantity. Not just the important part but
    // all so as to have a sanity check.
    // For all nodes.
    for (int t_node = 0; t_node < nnodes; t_node++) {
        // Calculate nominator.
        obj_nominator = red_absorbing_probs[t_node].pagerank - nominator_const;
        obj_nominator *= ((1 - jump_prob) / jump_prob);
        // Calculate denominator.
        obj_denominator = source_absorbing_probs[t_node].pagerank - denominator_const;
        obj_denominator *= ((1 - jump_prob) / jump_prob);
        obj_denominator = s_out_degree + 1 - obj_denominator;
        objective_val[t_node].node_id = t_node;
        objective_val[t_node].pagerank = red_pagerank + rank_vector[s_node].pagerank * (obj_nominator / obj_denominator);
        // Theory check print.
        if (obj_denominator < 0) std::cout << "!!!NEGATIVE DENOMINATOR!!!\n";
    }

    return objective_val;
}

void Edge_addition::save_logs(std::string algo_name, std::vector<step_log> log_vec) {
    // Declare local variables.
    int records = log_vec.size();

    // Open log file.
    std::ofstream log_file("out_" + algo_name + "_log.txt");

    // Write logs to file.
    log_file << "Edge_Addition\tPagerank\tPrediction\n";
    for (int i = 0; i < records; i++) {
        log_file << i << "\t" << log_vec[i].red_pagerank << "\t" << log_vec[i].red_pagerank_prediction << std::endl;
    }

    // Close file.
    log_file.close();
}

void Edge_addition::save_logs_per_node(std::string algo_name, int src_node, std::vector<step_log> log_vec) {
    // Declare local variables.
    int records = log_vec.size();

    // Open log file.
    std::ofstream log_file("out_" + algo_name + "_" + std::to_string(src_node) + "_log.txt");

    // Write logs to file.
    log_file << "New Edges\tPg\tPr\n";
    for (int i = 0; i < records; i++) {
        log_file << i << "\t" << log_vec[i].red_pagerank << "\t" << log_vec[i].red_pagerank_prediction << std::endl;
    }

    // Close file.
    log_file.close();
}