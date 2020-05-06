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
Edge_addition::Edge_addition(graph &g, pagerank_algorithms &algs, int n_source, int n_target, int n_edges) : g(g), algs(algs),
    n_source(n_source), n_target(n_target), n_edges(n_edges) {
        // Nothing to do here.
}

// Algorithms
void Edge_addition::source_heuristic(const double C, const double eps, const int max_iter) {
    std::cout << "Source_heuristic!\n";
    switch (l_kind)
    {
    case log_kind::ALL_EDGES:
        source_heuristic_all_sources(C, eps, max_iter);
        break;
    case log_kind::PER_EDGE:
        source_heuristic_per_source(C, eps, max_iter);
        break;
    default:
        exit(0);
        break;
    }
}

void Edge_addition::source_heuristic_per_source(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v rank_vector;
    std::vector<int> s_nodes;
    std::vector<step_log> log_vec;
    pagerank_t target_node;
    std::vector<edge> new_edges;
    edge new_edge;
    double red_pagerank;
    int no_edges;

    // Get source nodes.
    s_nodes = get_source_nodes(n_source, C, eps, max_iter);
    // Save source nodes.
    save_vector("out_sources_one.txt", s_nodes);

    // For each Source nodes.
    for (auto s_node = s_nodes.begin(); s_node < s_nodes.end(); s_node++) {// Clear log_vector
        log_vec.clear();
        // Get pagerank.
        rank_vector = algs.get_pagerank(C, eps, max_iter);
        // Get Red pagerank.
        red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
        // Add log point to log vector.
        add_log_point(log_vec, red_pagerank, red_pagerank);
        // Update new edge source.
        new_edge.source = *s_node;
        // Find valide number of new edges.
        no_edges = get_valide_no_edges(new_edge.source);
        // Find n_edges nodes to connect.
        for (int t_node = 0; t_node < no_edges; t_node++) {
            // Get best target node.
            target_node = get_target_nodes(new_edge.source, 1)[0];
            // Update new edge destination.
            new_edge.destination = target_node.node_id;
            // Add best node to source node.
            g.add_edge(new_edge.source, new_edge.destination);
            // Add edge to new edges.
            new_edges.push_back(new_edge);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            add_log_point(log_vec, red_pagerank, target_node.pagerank);
        }
        // Save logs.
        save_vector("out_greedy_source_" + std::to_string(new_edge.source) + ".txt", log_vec);
        // Remove edges.
        remove_new_edges(new_edges);
    }
}

void Edge_addition::source_heuristic_all_sources(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v rank_vector;
    std::vector<int> s_nodes;
    std::vector<step_log> log_vec;
    std::vector<edge> new_edges;
    edge new_edge;
    pagerank_t target_node;
    double red_pagerank;
    int no_edges;

    // Get source nodes.
    s_nodes = get_source_nodes(n_source, C, eps, max_iter);
    // Save source nodes.
    save_vector("out_sources_all.txt", s_nodes);
    // Get pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Get Red pagerank.
    red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Add log point to log vector.
    add_log_point(log_vec, red_pagerank, red_pagerank);

    // For each Source nodes.
    int k = 0;
    for (auto s_node = s_nodes.begin(); s_node < s_nodes.end(); s_node++) {
        std::cout << ++k << " source\n";
        // Find valide number of new edges.
        no_edges = get_valide_no_edges(*s_node);
        new_edge.source = *s_node;
        // Find n_edges nodes to connect.
        for (int t_node = 0; t_node < no_edges; t_node++) {
            // Get best node.
            target_node = get_target_nodes(new_edge.source, 1)[0];
            new_edge.destination = target_node.node_id;
            // Add best node to source node.
            g.add_edge(new_edge.source, new_edge.destination);
            // Add edge to new edges.
            new_edges.push_back(new_edge);
            // Get pagerank.
            rank_vector = algs.get_pagerank(C, eps, max_iter);
            // Get Red pagerank.
            red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
            // Renew log point.
            add_log_point(log_vec, red_pagerank, target_node.pagerank);
        }
    }
    // Save log vector.
    save_vector("out_greedy_all.txt", log_vec);
    // Remove new edges.
    remove_new_edges(new_edges);
}

void Edge_addition::edge_heuristic(const double C, const double eps, const int max_iter) {
    std::cout << "Edge heuristic\n";
    if (l_kind == log_kind::ALL_EDGES) {
        edge_heuristic_all(C, eps, max_iter);
    } else if (l_kind == log_kind::PER_SOURCE) {
        edge_heuristic_per_one(C, eps, max_iter);
    }
}

void Edge_addition::edge_heuristic_per_one(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v rank_vector;
    std::vector<int> s_nodes;
    std::vector<step_log> log_vec;
    std::vector<edge> new_edges;
    double red_pagerank;

    // Init seed.
    srand(time(NULL));
    // Get pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Get Red pagerank.
    red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Add log point to log vector.
    add_log_point(log_vec, red_pagerank, red_pagerank);
    // Get edges.
    new_edges = get_edges(n_edges);

    // Add edges.
    for (auto new_edge = new_edges.begin(); new_edge < new_edges.end(); new_edge++) {
        g.add_edge(new_edge -> source, new_edge -> destination);
        // Get pagerank.
        rank_vector = algs.get_pagerank(C, eps, max_iter);
        // Get Red pagerank.
        red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
        // Add log point to log vector.
        add_log_point(log_vec, red_pagerank);
        g.remove_edge(new_edge -> source, new_edge -> destination);
    }
    // Save logs.
    save_vector("out_random_edges_per_one.txt", log_vec);
}

void Edge_addition::edge_heuristic_all(const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v rank_vector;
    std::vector<int> s_nodes;
    std::vector<step_log> log_vec;
    std::vector<edge> new_edges;
    double red_pagerank;

    // Init seed.
    srand(time(NULL));
    // Get pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Get Red pagerank.
    red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
    // Add log point to log vector.
    add_log_point(log_vec, red_pagerank);
    
    // Get edges.
    new_edges = get_edges(n_edges);
    if (e_criterion == edge_criterion::RANDOM) {
        save_vector("out_random_edges.txt", new_edges);
    } else if (e_criterion == edge_criterion::PRODUCT) {
        save_vector("out_one_edges.txt", new_edges);
    } else if (e_criterion == edge_criterion::SUM) {
        save_vector("out_two_edges.txt", new_edges);
    } else {
        std::cout << "Not supported edge criterion!\n";
        exit(0);
    }
    // Add edges.
    int k = 0;
    //std::cout << "size-------------: " << new_edges.size();
    for (auto new_edge = new_edges.begin(); new_edge < new_edges.end(); new_edge++) {
        std::cout << ++k << " edge\n";
        g.add_edge(new_edge -> source, new_edge -> destination);
        // Get pagerank.
        rank_vector = algs.get_pagerank(C, eps, max_iter);
        // Get Red pagerank.
        red_pagerank = g.get_pagerank_per_community(rank_vector)[1];
        // Add log point to log vector.
        add_log_point(log_vec, red_pagerank);
    }
    // Remove new edges.
    remove_new_edges(new_edges);
    if (e_criterion == edge_criterion::RANDOM) {
        save_vector("out_random_edges_all.txt", log_vec);
    } else if (e_criterion == edge_criterion::PRODUCT) {
        save_vector("out_one_edges_all.txt", log_vec);
    } else if (e_criterion == edge_criterion::SUM) {
        save_vector("out_two_edges_all.txt", log_vec);
    } else {
        std::cout << "Not supported edge criterion!\n";
        exit(0);
    }
    
}

// -------------------------------------------------------------------- Help Functions -------------------------------------------------------------------------.

// Get source nodes.
std::vector<int> Edge_addition::get_source_nodes(int n, const double C, const double eps, const int max_iter) {
    if (s_criterion == source_criterion::RANDOM) {
        return get_source_nodes_random(n);
    } else if (s_criterion == source_criterion::CRITERION_ONE) {
        return get_source_nodes_one(n, C, eps, max_iter);
    } else {
        std::cout << "Not supported source selection criterion!\n";
        exit(0);
    }
}

std::vector<int> Edge_addition::get_source_nodes_random(int n) {
    std::vector<int> s_nodes;
    int s_node;
    bool is_source;
    int no_nodes = g.get_num_nodes();

    // Get random source nodes.
    for (int i = 0; i < n; i++) {
        do {
            // Get random int from 0 to nnodes - 1.
            s_node = (rand() % no_nodes);
            // Check if already exists.
            if (std::find(s_nodes.begin(), s_nodes.end(), s_node) != s_nodes.end()) {
                is_source = true;
            } else {
                is_source = false;
            }
        } while (is_source);
        // Add node to sources.
        s_nodes.push_back(s_node);
    }

    return s_nodes;
}

std::vector<int> Edge_addition::get_source_nodes_one(int n, const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v rank_vector, temp_s_nodes;
    std::vector<int> s_nodes(n);
    int src_node;

    // Get Pagerank.
    rank_vector = algs.get_pagerank(C, eps, max_iter);
    // Sort pagerank in descentding order.
    algs.sort_pagerank_vector(rank_vector);
    // Resize top_source_vector to 2*n. Keep top 2*n.
    temp_s_nodes.resize(2*n);
    for (int i = 0; i < 2*n; i++) {
        temp_s_nodes[i] = rank_vector[i];
    }
    
    // Get Blue personilized pagerank.
    rank_vector = algs.get_blue_abs_prob(C, eps, max_iter);
    // Change top source pagerank to Blue personilized.
    for (int i = 0; i < 2*n; i++) {
        src_node = temp_s_nodes[i].node_id;
        temp_s_nodes[i].pagerank = rank_vector[src_node].pagerank;
    }
    // Sort top nodes by Blue pagerank.
    algs.sort_pagerank_vector(temp_s_nodes);
    // Keep n top source nodes by this ranking.
    temp_s_nodes.resize(n);
    // Convert to return.
    for (int i = 0; i < n; i++) {
        s_nodes[i] = temp_s_nodes[i].node_id;
    }

    return s_nodes;
}

// Get target nodes.
pagerank_v Edge_addition::get_target_nodes(int s_node, int no_targets) {
    if (t_criterion == target_criterion::RANDOM) {
        return get_target_nodes_random(s_node, no_targets);
    } else if (t_criterion == target_criterion::STEP_PREDICTION) {
        return get_target_nodes_pred(s_node,no_targets);
    } else {
        std::cout << "Not supported target selection criterion!\n";
        exit(0);
    }
}

pagerank_v Edge_addition::get_target_nodes_random(int s_node, int no_targets) {
    std::vector<int> neighbors, all_nodes, no_neighbors;
    pagerank_v t_nodes(no_targets);
    int no_nodes = g.get_num_nodes();

    // Init all_nodes.
    for (int i = 0; i < no_nodes; i++) {
        all_nodes.push_back(i);
    }
    // Get neighbors of source node.
    neighbors = g.get_out_neighbors(s_node);
    // Sort vectors for difference.
    std::sort(neighbors.begin(), neighbors.end());
    // Get difference.
    std::set_difference(all_nodes.begin(), all_nodes.end(), neighbors.begin(), neighbors.end(), std::back_inserter(no_neighbors));
    // Suffle random.
    std::random_shuffle(no_neighbors.begin(), no_neighbors.end());
    if (no_neighbors.size() < (unsigned)no_targets) {
        std::cout << "No valide number of target for the source!\n";
        exit(0);
    }
    // Keep only no_targets.
    no_neighbors.resize(no_targets);
    // Convert to pagerank_v
    for (int i = 0; i < no_targets; i++) {
        t_nodes[i].node_id = no_neighbors[i];
    }

    return t_nodes;
}

pagerank_v Edge_addition::get_target_nodes_pred(int s_node, int no_targets) {
    pagerank_v t_nodes;
    std::vector<int> s_neighbors;
    pagerank_t cand;
    // Get objective value for all nodes.
    pagerank_v obj_values = get_objective_val(s_node);
    // Remove from comparison neighbors and source by setting
    // their value to -1.
    obj_values[s_node].pagerank = -1;
    s_neighbors = g.get_out_neighbors(s_node);
    for (auto it = s_neighbors.begin(); it < s_neighbors.end(); it++) {
        obj_values[*it].pagerank = -1;
    }
    // Get no_edges bests.
    for (int i = 0; i < no_targets; i++) {
        cand.node_id = -1;
        cand.pagerank = -1;
        // Search for the best.
        for (auto it = obj_values.begin(); it < obj_values.end(); it++) {
            if (it -> pagerank > cand.pagerank) {
                cand.node_id = it -> node_id;
                cand.pagerank = it -> pagerank;
            }
        }
        // Add best to target nodes.
        t_nodes.push_back(cand);
        // Remove best from competition.
        obj_values[cand.node_id].pagerank = -1;
    }
    
    return t_nodes;
}

// Get random edges.
std::vector<edge> Edge_addition::get_edges(int no_edges, const double C, const double eps, const int max_iter) {
    if (e_criterion == edge_criterion::RANDOM) {
        return get_edges_random(no_edges);
    } else if (e_criterion == edge_criterion::PRODUCT) {
        return get_edges_one(no_edges, C, eps, max_iter);
    } else if (e_criterion == edge_criterion::SUM) {
        return get_edges_two(no_edges, C, eps, max_iter);
    } else if (e_criterion == edge_criterion::FORMULA) {
        return get_edges_three(no_edges, C, eps, max_iter);
    } else {
        std::cout << "Edge criterion not supported!\n";
        exit(0);
    }
}

std::vector<edge> Edge_addition::get_edges_random(int no_edges) {
    std::cout << "Edges random\n";
    std::vector<edge> new_edges;
    edge new_edge;
    int no_valide_edges, j;

    j = 0;
    for (int i = 0; i < no_edges; i++) {
        // Get random source from 0 to nnodes - 1.
        new_edge.source = get_source_nodes(1)[0];
        no_valide_edges = get_valide_no_edges(new_edge.source);
        if (no_valide_edges > 0) {
            // Get random target.
            new_edge.destination = get_target_nodes(new_edge.source, 1)[0].node_id;
            // if new already exists in new edges.
            if (std::find(new_edges.begin(), new_edges.end(), new_edge) != new_edges.end()) {
                i--;
            } else {
                new_edges.push_back(new_edge);
            }
        } else {
            i--;
        }
        j++;
        if (j > 3 * no_edges) {
            std::cout << "3 x number of edges tries without result!\n";
            exit(0);
        }
    }
 
    save_vector("out_edges_random.txt", new_edges);   
    return new_edges;
}

std::vector<edge> Edge_addition::get_edges_one(int no_edges, const double C, const double eps, const int max_iter) {
    std::cout << "Edges one\n";
    std::vector<edge> new_edges;
    std::vector<int> s_neighbors;
    edge new_edge;
    pagerank_v pagerank, red_abs_probs;
    std::vector<double> obj_val(no_edges * no_edges, 0);
    int row, b_row, b_col;
    double max;
    bool is_nei;

    // Get quantities.
    pagerank = algs.get_pagerank(C, eps, max_iter);
    algs.sort_pagerank_vector(pagerank);
    pagerank.resize(no_edges);
    red_abs_probs = algs.get_red_abs_prob(C, eps, max_iter);
    algs.sort_pagerank_vector(red_abs_probs);
    red_abs_probs.resize(no_edges);

    // Calculate criterion.
    for (int i = 0; i < no_edges; i++) {
        row = no_edges * i;
        for (int j = 0; j < no_edges; j++) {
            s_neighbors = g.get_out_neighbors(pagerank[i].node_id);
            // If the edge already exist is true.
            is_nei = (std::find(s_neighbors.begin(), s_neighbors.end(), red_abs_probs[j].node_id) == s_neighbors.end()) ? false : true;
            if (is_nei) {
                obj_val[row + j] = -1;
            } else {
                obj_val[row + j] = pagerank[i].pagerank * red_abs_probs[j].pagerank;
            }
        }
    }

    // Get edges.
    for (int e = 0; e < no_edges; e++) {
        // Find best edge.
        max = 0;
        b_row = 0;
        b_col = 0;
        for (int i = 0; i < no_edges; i++) {
            row = no_edges * i;
            for (int j = 0; j < no_edges; j++) {
                if (max < obj_val[row + j]) {
                    max = obj_val[row + j];
                    b_row = row;
                    b_col = j;
                    new_edge.source = pagerank[i].node_id;
                    new_edge.destination = red_abs_probs[j].node_id;
                }
            }
        }
        // Add it to new edges.
        new_edges.push_back(new_edge);
        // Remove it from next search.
        obj_val[b_row + b_col] = 0;
    }

    save_vector("out_edges_one.txt", new_edges);
    return new_edges;
}

std::vector<edge> Edge_addition::get_edges_two(int no_edges, const double C, const double eps, const int max_iter) {
    std::cout << "Edges two\n";
    std::vector<edge> new_edges;
    std::vector<int> s_neighbors;
    edge new_edge;
    pagerank_v pagerank, red_abs_probs;
    std::vector<double> obj_val(no_edges * no_edges, 0);
    int row, b_row, b_col;
    double max;
    bool is_nei;

    // Get quantities.
    pagerank = algs.get_pagerank(C, eps, max_iter);
    algs.sort_pagerank_vector(pagerank);
    pagerank.resize(no_edges);
    red_abs_probs = algs.get_red_abs_prob(C, eps, max_iter);
    algs.sort_pagerank_vector(red_abs_probs);
    red_abs_probs.resize(no_edges);

    // Calculate criterion.
    for (int i = 0; i < no_edges; i++) {
        row = no_edges * i;
        for (int j = 0; j < no_edges; j++) {
            s_neighbors = g.get_out_neighbors(pagerank[i].node_id);
            // If the edge already exist is true.
            is_nei = (std::find(s_neighbors.begin(), s_neighbors.end(), red_abs_probs[j].node_id) == s_neighbors.end()) ? false : true;
            if (is_nei) {
                obj_val[row + j] = -1;
            } else {
                obj_val[row + j] = pagerank[i].pagerank + red_abs_probs[j].pagerank;
            }
        }
    }

    // Get edges.
    for (int e = 0; e < no_edges; e++) {
        // Find best edge.
        max = 0;
        b_row = 0;
        b_col = 0;
        for (int i = 0; i < no_edges; i++) {
            row = no_edges * i;
            for (int j = 0; j < no_edges; j++) {
                if (max < obj_val[row + j]) {
                    max = obj_val[row + j];
                    b_row = row;
                    b_col = j;
                    new_edge.source = pagerank[i].node_id;
                    new_edge.destination = red_abs_probs[j].node_id;
                }
            }
        }
        // Add it to new edges.
        new_edges.push_back(new_edge);
        // Remove it from next search.
        obj_val[b_row + b_col] = 0;
    }

    save_vector("out_edges_two.txt", new_edges);
    return new_edges;
}

std::vector<edge> Edge_addition::get_edges_three(int no_edges, const double C, const double eps, const int max_iter) {
    std::cout << "Edges three\n";
    std::vector<edge> new_edges;
    std::vector<int> s_neighbors;
    edge new_edge;
    pagerank_v pagerank, red_abs_probs;
    std::vector<double> obj_val(no_edges * no_edges, 0);
    int row, b_row, b_col;
    double max;
    bool is_nei;

    // Get quantities.
    pagerank = algs.get_pagerank(C, eps, max_iter);
    algs.sort_pagerank_vector(pagerank);
    pagerank.resize(no_edges);
    red_abs_probs = algs.get_red_abs_prob(C, eps, max_iter);
    algs.sort_pagerank_vector(red_abs_probs);
    red_abs_probs.resize(no_edges);

    // Calculate criterion.
    for (int i = 0; i < no_edges; i++) {
        row = no_edges * i;
        for (int j = 0; j < no_edges; j++) {
            s_neighbors = g.get_out_neighbors(pagerank[i].node_id);
            // If the edge already exist is true.
            is_nei = (std::find(s_neighbors.begin(), s_neighbors.end(), red_abs_probs[j].node_id) == s_neighbors.end()) ? false : true;
            if (is_nei) {
                obj_val[row + j] = -1;
            } else {
                obj_val[row + j] = pagerank[i].pagerank + red_abs_probs[j].pagerank;
            }
        }
    }

    // Get edges.
    for (int e = 0; e < no_edges; e++) {
        // Find best edge.
        max = 0;
        b_row = 0;
        b_col = 0;
        for (int i = 0; i < no_edges; i++) {
            row = no_edges * i;
            for (int j = 0; j < no_edges; j++) {
                if (max < obj_val[row + j]) {
                    max = obj_val[row + j];
                    b_row = row;
                    b_col = j;
                    new_edge.source = pagerank[i].node_id;
                    new_edge.destination = red_abs_probs[j].node_id;
                }
            }
        }
        // Add it to new edges.
        new_edges.push_back(new_edge);
        // Remove it from next search.
        obj_val[b_row + b_col] = 0;
    }

    save_vector("out_edges_three.txt", new_edges);
    return new_edges;
}

// Remove new edges.
void Edge_addition::remove_new_edges(std::vector<edge> &new_edges) {
    for (auto it = new_edges.begin(); it < new_edges.end(); it++) {
        g.remove_edge(it -> source, it -> destination);
    }
    new_edges.clear();
}

// Get Predictions.
pagerank_v Edge_addition::get_objective_val(int s_node, const double C, const double eps, const int max_iter) {
    // Declare local variables.
    pagerank_v objective_val, rank_vector, red_absorbing_probs, source_absorbing_probs;
    std::vector<int> neighbors;
    double red_pagerank, nominator_const, denominator_const, obj_nominator, obj_denominator;
    double jump_prob = 1 - C;
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

double Edge_addition::get_gen_objective_val(int s_node, std::vector<edge> new_edges, const double C)
{
    // Declare local variables.
    std::vector<edge> pred_edges = new_edges;
    pagerank_v pagerank, red_abs_probs, s_abs_probs;
    double nominator_const, denominator_const, temp_const, objective_val, red_pagerank;
    std::vector<int> s_neighbors;
    double jump_prob = 1 - C;
    int neighbor;
    int s_out_degree = g.get_out_degree(s_node);
    int no_edges = new_edges.size();
    int no_nodes = g.get_num_nodes();

    // Probably useless.
    if (no_edges <= 0) {
        std::cout << "No new edges, Unexpected behaviour\t";
        temp_const = 0;
    }

    // Remove edges from graph to calculate initial quantinties.
    remove_new_edges(new_edges);
    // Get initial pageranks, reb abs probs, source abs probs, ...
    pagerank = algs.get_pagerank();
    red_abs_probs = algs.get_red_abs_prob();
    s_abs_probs = algs.get_node_abs_prob(s_node);
    red_pagerank = g.get_pagerank_per_community(pagerank)[1];
    // Get source neighbors.
    s_neighbors = g.get_out_neighbors(s_node);

    // Get average Red pagerank of initial neighbors for nominator.
    nominator_const = 0;
    if (s_out_degree > 0) {
        for (int nei = 0; nei < s_out_degree; nei++) {
            neighbor = s_neighbors[nei];
            nominator_const += red_abs_probs[neighbor].pagerank;
        }
        nominator_const *= (1 / (float)s_out_degree);
    } else {
        for (int nei = 0; nei < no_nodes; nei++) {
            neighbor = nei;
            nominator_const += red_abs_probs[neighbor].pagerank;
        }
        nominator_const *= (1 / (float)no_nodes);
    }
    // Get average Red Pagerank of new neighbors.
    for (int nei = 0; nei < no_edges; nei++) {
        neighbor = new_edges[nei].destination;
        temp_const += red_abs_probs[neighbor].pagerank;
    }
    temp_const *= (1 / (float)no_edges);
    nominator_const = temp_const - nominator_const;
    // Get average Source pagerank of neighbors for denominator.
    denominator_const = 0;
    if (s_out_degree > 0) {
        for (int nei = 0; nei < s_out_degree; nei++) {
            neighbor = s_neighbors[nei];
            denominator_const += s_abs_probs[neighbor].pagerank;
        }
        denominator_const *= (1 / (float)s_out_degree);
    } else {
        for (int nei = 0; nei < no_nodes; nei++) {
            neighbor = nei;
            denominator_const += s_abs_probs[neighbor].pagerank;
        }
        denominator_const *= (1 / (float)no_nodes);
    }
    // Get average Source pagerank of new neighbors.
    temp_const = 0;
    for (int nei = 0; nei < no_edges; nei++) {
        neighbor = new_edges[nei].destination;
        temp_const += s_abs_probs[neighbor].pagerank;
    }
    temp_const *= (1 / (float)no_edges);
    denominator_const = temp_const - denominator_const;
    // Calculate the Quantity. Not just the important part but
    // all so as to have a sanity check.
    nominator_const *= ((1 - jump_prob) / jump_prob);
    denominator_const *= ((1 - jump_prob) / jump_prob);
    denominator_const = ((s_out_degree + no_edges) / (float)no_edges) - denominator_const;
    objective_val = red_pagerank + (pagerank[s_node].pagerank * (nominator_const / denominator_const));
    // Theory check print.
    if (denominator_const < 0) std::cout << "!!!NEGATIVE DENOMINATOR!!!\n";

    return objective_val;
}

// Get valide edges.
int Edge_addition::get_valide_no_edges(int s_node) {
    int max_edges = (g.get_num_nodes() - 1) - g.get_out_degree(s_node);

    return std::min(n_target, max_edges);
}

// Save vectors.
void Edge_addition::save_vector(std::string file_name, pagerank_v &s_vector) {
    // Declare local variables.
    int n = s_vector.size();

    // Open log file.
    std::ofstream log_file(file_name);

    // Write logs to file.
    log_file << "Edge\tValue\n";
    for (int i = 0; i < n; i++) {
        log_file << s_vector[i].node_id << "\t" << s_vector[i].pagerank << std::endl;
    }

    // Close file.
    log_file.close();
}

void Edge_addition::save_vector(std::string file_name, std::vector<int> &s_vector) {
    // Declare Variables
    int n = s_vector.size();  

    std::ofstream log_file(file_name);
    // Write nodes.
    for (int i = 0; i < n; i++) {
        log_file << s_vector[i] << "\n";
    }
    // Close file.
    log_file.close();
}

void Edge_addition::save_vector(std::string file_name, std::vector<double> &s_vector) {
    // Declare Variables
    int n = s_vector.size();  

    std::ofstream log_file(file_name);
    // Write nodes.
    for (int i = 0; i < n; i++) {
        log_file << s_vector[i] << "\n";
    }
    // Close file.
    log_file.close();
}

void Edge_addition::save_vector(std::string file_name, std::vector<edge> &s_vector) {
    std::ofstream edge_file(file_name);
    int n = s_vector.size();
    for (int i = 0; i < n; i++) {
        edge_file << s_vector[i].source << "\t" << s_vector[i].destination << "\n";
    }
    edge_file.close();
}

void Edge_addition::save_vector(std::string file_name, std::vector<step_log> &s_vector) {
    // Declare local variables.
    int n = s_vector.size();

    // Open log file.
    std::ofstream log_file(file_name);

    // Write logs to file.
    log_file << "Edge\tPagerank\tPrediction\tSum_Prediction\n";
    for (int i = 0; i < n; i++) {
        log_file << i << "\t" << s_vector[i].red_pagerank << "\t" << s_vector[i].red_pagerank_prediction << "\t" << s_vector[i].red_pagerank_generalized_prediction << std::endl;
    }

    // Close file.
    log_file.close();
}

void Edge_addition::add_log_point(std::vector<step_log> &log_vec, double red_pagerank, double prediction, double gen_prediction) {
    step_log log_point;
    log_point.red_pagerank = red_pagerank;
    log_point.red_pagerank_prediction = prediction;
    log_point.red_pagerank_generalized_prediction = gen_prediction;
    log_vec.push_back(log_point);
}

// Setters.
void Edge_addition::set_criterion(source_criterion s_crit) {
    s_criterion = s_crit;
}

void Edge_addition::set_criterion(target_criterion t_crit) {
    t_criterion = t_crit;
}

void Edge_addition::set_criterion(edge_criterion e_crit) {
    e_criterion = e_crit;
}

void Edge_addition::set_criterion(prediction_kind p_crit) {
    p_kind = p_crit;
}

void Edge_addition::set_criterion(log_kind l_crit) {
    l_kind = l_crit;
}