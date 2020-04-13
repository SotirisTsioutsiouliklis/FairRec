#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "edge_add.hpp"


Edge_addition::Edge_addition(graph &g, pagerank_algorithms &algs, int n_source, int n_target) : g(g), algs(algs), n_source(n_source),
n_target(n_target) {
    // Fix size for the top nodes.
    top_source_nodes.resize(2*n_source);
    log_vec.resize(n_source * n_target + 1);
    
}

void Edge_addition::greedy() {
    // Get source nodes
    get_best_source_nodes(n_source);
    int nnodes = g.get_num_nodes;
    for (int s_node = 0; s_node < n_source ; s_node++) {
        top_target_nodes = get_best_target_nodes(top_source_nodes[s_node].node_id, n_target);
        for (int t_node = 0; t_node < n_target; t_node++) {
            // Get pagerank.
            rank_vector = algs.get_pagerank();
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Store to log vector.
            log_vec[s_node + t_node].red_pagerank = red_pagerank;
            // Find best edge to add.
            objective_val = get_objective_val(s_node);
            // Remove neighbors of source node from procedure.
            neighbors = g.get_out_neighbors;
            for (int nei = 0; nei < g.get_out_degree(s_node); nei++) {  
                objective_val[neighbors[nei]].pagerank = 0;
            }
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

void Edge_addition::get_best_source_nodes(int n) {
    // Get Pagerank.
    rank_vector = algs.get_pagerank();
    // Sort pagerank in descentding order.
    algs.sort_pagerank_vector(rank_vector);
    // Keep top 2*n.
    for (int i = 0; i < 2*n; i++) {
        top_source_nodes[i] = rank_vector[i];
    }
    // Get Blue personilized pagerank.
    rank_vector = algs.get_pers_pagerank();
    // Change top source pagerank to Blue personilized.
    for (int i = 0; i < 2*n; i++) {
        top_source_nodes[i].pagerank = rank_vector[top_source_nodes[i].node_id].pagerank;
    }
    // Sort top nodes by Blue pagerank.
    algs.sort_pagerank_vector(top_source_nodes);
    // Keep 10 top in this ranking.
    top_source_nodes.resize(n);
}

pagerank_v Edge_addition::get_best_target_nodes(int u, int k) {
    
}

