#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "edge_add.hpp"

// Class constructor.
Edge_addition::Edge_addition(graph &g, pagerank_algorithms &algs, int n_source, int n_target) : g(g), algs(algs),
    n_source(n_source), n_target(n_target) {
    // Nothing to do here.
}

void Edge_addition::greedy() {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, source_nodes;
    std::vector<int> neighbors;

    pagerank_s target_node;
    double red_pagerank;
    int s_out_degree, src_node;

    // Get source nodes.
    source_nodes = get_best_source_nodes(n_source);
    // Get number of nodes.
    int nnodes = g.get_num_nodes;
    // For each Source nodes.
    for (int s_node = 0; s_node < n_source ; s_node++) {
        // Print Source node. Test print to know it runs.
        std::cout << "Sourece node: " << s_node << std::endl;
        // Get source node id.
        src_node = source_nodes[s_node].node_id;
        // Get out degree for source node.
        s_out_degree = g.get_out_degree(src_node);
        // FInd n_target nodes to connect.
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get pagerank.
            rank_vector = algs.get_pagerank();
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Store red pagerank to log vector.
            log_vec[s_node + t_node].red_pagerank = red_pagerank;
            // Get objective values for all nodes.
            objective_val = get_objective_val(src_node);
            // Remove neighbors of source node from finding the
            // best procedure. Do that by zeroing their objective value.
            neighbors = g.get_out_neighbors(src_node);
            for (int nei = 0; nei < s_out_degree; nei++) {  
                objective_val[neighbors[nei]].pagerank = 0;
            }
            // Remove self from finding the best procedure.
            // Do that by zeroing its objective value.
            objective_val[src_node].pagerank = 0;
            // Find best edge to add. Assume best is node 0.
            target_node.node_id = 0;
            target_node.pagerank = objective_val[0].pagerank;
            // Search all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is better, Renew node information.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add prediction to log.
            log_vec[s_node + t_node].red_pagerank_prediction = get_prediction(target_node);
            // Add best node to source node.
            g.add_edge(s_node, target_node.node_id);
        }
    }
    // Get pagerank
    rank_vector = algs.get_pagerank();
    // Write last log informations.
    log_vec[n_source + n_target].red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    log_vec[n_source + n_target].red_pagerank_prediction = 0;
}

void Edge_addition::fast_greedy() {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, source_nodes;
    std::vector<int> neighbors;

    pagerank_s target_node;
    double red_pagerank;
    int s_out_degree, src_node;

    // Get source nodes.
    source_nodes = get_best_source_nodes(n_source);
    // Get number of nodes.
    int nnodes = g.get_num_nodes;
    for (int s_node = 0; s_node < n_source ; s_node++) {
        // Print source node. Test print to know it runs.
        std::cout << "Sourece node: " << s_node << std::endl;
        // Get source node id.
        src_node = source_nodes[s_node].node_id;
        // Get out degree for source node.
        s_out_degree = g.get_out_degree(src_node);
        // Get objective values.
        objective_val = get_objective_val(src_node);
        // Remove neighbors of source node from finding the
        // best procedure. Do that by zeroing their objective value.
        neighbors = g.get_out_neighbors(src_node);
        for (int nei = 0; nei < s_out_degree; nei++) {  
            objective_val[neighbors[nei]].pagerank = 0;
        }
        // Remove self from finding the best procedure.
        // Do that by zeroing its objective value.
        objective_val[s_node].pagerank = 0;
        // Find n_target nodes to connect.
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get pagerank.
            rank_vector = algs.get_pagerank();
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Store pagerank to log vector.
            log_vec[s_node + t_node].red_pagerank = red_pagerank;
            // Find best edge to add. Assume 0 to eb the best.
            target_node.node_id = 0;
            target_node.pagerank = objective_val[0].pagerank;
            // Check all nodes.
            for (int cand = 0; cand < nnodes; cand++) {
                // If there is a better node, renew target node informations.
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add prediction to log.
            log_vec[s_node + t_node].red_pagerank_prediction = get_prediction(target_node);
            // Add best node to source node.
            g.add_edge(s_node, target_node.node_id);
            // Remove this node from procedure. Do that by
            // zeroing its objective value.
            objective_val[target_node.node_id].node_id = 0;
        }
    }
    // Get pagerank.
    rank_vector = algs.get_pagerank();
    // Write last log informations.
    log_vec[n_source + n_target].red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    log_vec[n_source + n_target].red_pagerank_prediction = 0;
}

pagerank_v Edge_addition::get_best_source_nodes(int n) {
    // Declare local variables.
    pagerank_v rank_vector, top_source_nodes;
    int src_node;

    // Get Pagerank.
    rank_vector = algs.get_pagerank();
    // Sort pagerank in descentding order.
    algs.sort_pagerank_vector(rank_vector);
    // Resize top_source_vector to 2*n. Keep top 2*n.
    top_source_nodes.resize(2*n);
    for (int i = 0; i < 2*n; i++) {
        top_source_nodes[i] = rank_vector[i];
    }
    // Get Blue personilized pagerank.
    rank_vector = algs.get_blue_abs_prob();
    // Change top source pagerank to Blue personilized.
    for (int i = 0; i < 2*n; i++) {
        src_node = top_source_nodes[i].node_id;
        top_source_nodes[i].pagerank = rank_vector[src_node].pagerank;
    }
    // Sort top nodes by Blue pagerank.
    algs.sort_pagerank_vector(top_source_nodes);
    // Keep n top source nodes by this ranking.
    top_source_nodes.resize(n);

    return top_source_nodes;
}

pagerank_v Edge_addition::get_objective_val(int s_node) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, red_absorbing_probs, source_absorbing_probs;
    std::vector<int> neighbors;
    double red_pagerank, nominator_const, denominator_const, obj_nominator, obj_denominator;
    int s_out_degree, neighbor, nnodes;

    // Get source out degree.
    s_out_degree = g.get_out_degree(s_node);
    // Run pagerank.
    rank_vector = algs.get_pagerank();
    // Get red pagerank.
    red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Run absoring to Red.
    red_absorbing_probs = algs.get_red_abs_prob();
    // Run absorbing to source.
    source_absorbing_probs = algs.get_node_abs_prob(s_node); // ----------- Pending implementaion.
    // Get source neighbors.
    neighbors = g.get_out_neighbors(s_node);
    // Get average Red pagerank of neighbors for nominator.
    nominator_const = 0;
    for (int nei = 0; nei < s_out_degree; nei++) {
        neighbor = neighbors[nei];
        nominator_const += red_absorbing_probs[neighbor].pagerank;
    }
    nominator_const *= (1 / s_out_degree);
    // Get average Source pagerank of neighbors for denominator.
    denominator_const = 0;
    for (int nei = 0; nei < s_out_degree; nei++) {
        neighbor = neighbors[nei];
        denominator_const += source_absorbing_probs[neighbor].pagerank;
    }
    denominator_const *= (1 / s_out_degree);
    // Calculate the Quantity. Not just the important part but
    // All so as to have a sanity check.
    nnodes = g.get_num_nodes;
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
    }

    return objective_val;
}

void Edge_addition::save_logs() {
    // Declare local variables.
    int records = log_vec.size();

    // Open log file.
    std::ofstream log_file("log.txt");

    // Write logs to file.
    log_file << "Pagerank\tPrediction\n";
    for (int i = 0; i < records; i++) {
        log_file << log_vec[i].red_pagerank << "\t\t" << log_vec[i].red_pagerank_prediction << std::endl;
    }

    // Close file.
    log_file.close();
}