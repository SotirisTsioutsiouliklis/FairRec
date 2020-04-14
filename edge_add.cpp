#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "edge_add.hpp"


Edge_addition::Edge_addition(graph &g, pagerank_algorithms &algs, int n_source, int n_target) : g(g), algs(algs),
    n_source(n_source), n_target(n_target) {
    // Nothing to do here.
}

void Edge_addition::greedy() {
    pagerank_v objective_val, rank_vector, source_nodes;
    std::vector<int> neighbors;

    pagerank_s target_node;
    double red_pagerank;
    int n_out_nei;

    // Get source nodes
    source_nodes = get_best_source_nodes(n_source);
    int nnodes = g.get_num_nodes;
    for (int s_node = 0; s_node < n_source ; s_node++) {
        // Get out degree for source node.
        n_out_nei = g.get_out_degree(source_nodes[s_node].node_id);
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get pagerank.
            rank_vector = algs.get_pagerank();
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Store to log vector.
            log_vec[s_node + t_node].red_pagerank = red_pagerank;
            // Renew rank metric.
            objective_val = get_objective_val(source_nodes[s_node].node_id);
            // Remove neighbors of source node from finding the
            // best procedure.
            neighbors = g.get_out_neighbors(source_nodes[s_node].node_id);
            for (int nei = 0; nei < n_out_nei; nei++) {  
                objective_val[neighbors[nei]].pagerank = 0;
            }
            // Remove self from finding the
            // best procedure.
            objective_val[s_node].pagerank = 0;
            // Find best edge to add.
            target_node.node_id = 0;
            target_node.pagerank = objective_val[0].pagerank;
            for (int cand = 0; cand < nnodes; cand++) {
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
    rank_vector = algs.get_pagerank();
    log_vec[n_source + n_target].red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    log_vec[n_source + n_target].red_pagerank_prediction = 0;
}

void Edge_addition::fast_greedy() {
    pagerank_v objective_val, rank_vector, source_nodes;
    std::vector<int> neighbors;

    pagerank_s target_node;
    double red_pagerank;
    int n_out_nei;

    // Get source nodes
    source_nodes = get_best_source_nodes(n_source);
    int nnodes = g.get_num_nodes;
    for (int s_node = 0; s_node < n_source ; s_node++) {
        // Get out degree for source node.
        n_out_nei = g.get_out_degree(source_nodes[s_node].node_id);
        // Get objective values.
        objective_val = get_objective_val(source_nodes[s_node].node_id);
        // Remove neighbors of source node from finding the
        // best procedure.
        neighbors = g.get_out_neighbors(source_nodes[s_node].node_id);
        for (int nei = 0; nei < n_out_nei; nei++) {  
            objective_val[neighbors[nei]].pagerank = 0;
        }
        // Remove self from finding the
        // best procedure.
        objective_val[s_node].pagerank = 0;
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get pagerank.
            rank_vector = algs.get_pagerank();
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Store to log vector.
            log_vec[s_node + t_node].red_pagerank = red_pagerank;
            // Find best edge to add.
            target_node.node_id = 0;
            target_node.pagerank = objective_val[0].pagerank;
            for (int cand = 0; cand < nnodes; cand++) {
                if (objective_val[cand].pagerank > target_node.pagerank) {
                    target_node.node_id = cand;
                    target_node.pagerank = objective_val[cand].pagerank;
                }
            }
            // Add prediction to log.
            log_vec[s_node + t_node].red_pagerank_prediction = get_prediction(target_node);
            // Add best node to source node.
            g.add_edge(s_node, target_node.node_id);
            // Remove this node from procedure.
            objective_val[target_node.node_id].node_id = 0;
        }
    }
    rank_vector = algs.get_pagerank();
    log_vec[n_source + n_target].red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    log_vec[n_source + n_target].red_pagerank_prediction = 0;
}

pagerank_v Edge_addition::get_best_source_nodes(int n) {
    pagerank_v rank_vector, top_source_nodes;

    // Get Pagerank.
    rank_vector = algs.get_pagerank();
    // Sort pagerank in descentding order.
    algs.sort_pagerank_vector(rank_vector);
    // Keep top 2*n.
    for (int i = 0; i < 2*n; i++) {
        top_source_nodes[i] = rank_vector[i];
    }
    // Get Blue personilized pagerank.
    rank_vector = algs.get_blue_abs_prob();
    // Change top source pagerank to Blue personilized.
    for (int i = 0; i < 2*n; i++) {
        top_source_nodes[i].pagerank = rank_vector[top_source_nodes[i].node_id].pagerank;
    }
    // Sort top nodes by Blue pagerank.
    algs.sort_pagerank_vector(top_source_nodes);
    // Keep n_source nodes top in this ranking.
    top_source_nodes.resize(n);

    return top_source_nodes;
}

pagerank_v Edge_addition::get_objective_val(int s_node) {
    pagerank_v objective_val, rank_vector, red_absorbing_probs, source_absorbing_probs;
    std::vector<int> neighbors;
    double red_pagerank, nominator_const, denominator_const, obj_nominator, obj_denominator;

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
    for (int nei = 0; nei < g.get_out_degree(s_node); nei++) {
        nominator_const += red_absorbing_probs[neighbors[nei]].pagerank;
    }
    nominator_const *= (1 / g.get_out_degree(s_node));
    // Get average Source pagerank of neighbors for denominator.
    denominator_const = 0;
    for (int nei = 0; nei < g.get_out_degree(s_node); nei++) {
        denominator_const += source_absorbing_probs[neighbors[nei]].pagerank;
    }
    denominator_const *= (1 / g.get_out_degree(s_node));
    // Calculate the Quantity. Not just the important part but
    // All so as to have a sanity check.
    int nnodes = g.get_num_nodes;
    // For all nodes.
    for (int t_node = 0; t_node < nnodes; t_node++) {
        // Calculate nominator.
        obj_nominator = red_absorbing_probs[t_node].pagerank - nominator_const;
        obj_nominator *= ((1 - jump_prob) / jump_prob);
        // Calculate denominator.
        obj_denominator = source_absorbing_probs[t_node].pagerank - denominator_const;
        obj_denominator *= ((1 - jump_prob) / jump_prob);
        obj_denominator = g.get_out_degree(s_node) - obj_denominator;
        objective_val[t_node].node_id = t_node;
        objective_val[t_node].pagerank = red_pagerank + rank_vector[s_node].pagerank * (obj_nominator / obj_denominator);
    }

    return objective_val;
}