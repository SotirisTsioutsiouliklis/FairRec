/**
 * This script runs 
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <set>
#include <cmath>
#include "graph.hpp"
#include "pagerank.hpp"
#include <chrono>
#include <omp.h>
#include <string>
#include <sstream>
#include <algorithm>

int main()
{
    omp_set_num_threads(20);
    
    int source_node = 10;
    int candidate_node;
    double network_fairness;
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::ofstream edge_file("testing-scores.txt");
    edge_file << "initial\tactual\tpredicted\n";

    network_fairness = g.get_pagerank_per_community(algs.get_pagerank())[1];

    // get addition predictions.
    pagerank_v objective_values = algs.getObjectiveValues(source_node);

    // Find node to add.
    std::vector<int> out_neigh = g.get_out_neighbors(10);
    for (int i = 0; i < g.get_num_nodes(); i++) {
        if (std::find(out_neigh.begin(), out_neigh.end(), i) != out_neigh.end()) {
            candidate_node = i;
            break;
        }
    }
    for (int i : g.get_out_neighbors(source_node)) {
		std::cout << i;
	}
	std::cout << "\n";
	for (int i : g.get_in_neighbors()) {
		std::cout << i;
	}
	std::cout << "\n";

    // Add edge and log result.
    g.add_edge(source_node, candidate_node);
    pagerank_v pagerank = algs.get_pagerank();
    double red_pagerank = g.get_pagerank_per_community(pagerank)[1];
    edge_file << network_fairness << "\t" << red_pagerank << "\t" << objective_values[candidate_node].pagerank << "\n";

    for (int i : g.get_out_neighbors(source_node)) {
		std::cout << i;
	}
	std::cout << "\n";
	for (int i : g.get_in_neighbors()) {
		std::cout << i;
	}
	std::cout << "\n";

    // remove edge to bring graph in initial state.
    g.remove_edge(source_node, candidate_node);
    network_fairness = g.get_pagerank_per_community(algs.get_pagerank())[1];

    for (int i : g.get_out_neighbors(source_node)) {
		std::cout << i;
	}
	std::cout << "\n";
	for (int i : g.get_in_neighbors()) {
		std::cout << i;
	}
	std::cout << "\n";

    // Find node to remove.
    for (int i : out_neigh) {
        if (g.get_out_neighbors(source_node).size() == 1 ) {
            break;
        }
        if (g.get_in_neighbors(i).size() <= 1) {
            continue;
        } else {
            candidate_node = i;
        }
    }

    for (int i : g.get_out_neighbors(source_node)) {
		std::cout << i;
	}
	std::cout << "\n";
	for (int i : g.get_in_neighbors()) {
		std::cout << i;
	}
	std::cout << "\n";

    // get objective values.
    pagerank_v neg_objective_values = algs.getDeletionObjectiveValues(source_node);

    // remove the edge and log the results.
    double candidate_pagerank;
    for (pagerank_t nn : neg_objective_values) {
        if (nn.node_id == candidate_node) {
            candidate_pagerank = nn.pagerank;
        }
    }
    g.remove_edge(source_node, candidate_node);
    pagerank = algs.get_pagerank();
    red_pagerank = g.get_pagerank_per_community(pagerank)[1];
    edge_file << network_fairness << "\t" << red_pagerank << "\t" << candidate_pagerank << "\n";
    
    for (int i : g.get_out_neighbors(source_node)) {
		std::cout << i;
	}
	std::cout << "\n";
	for (int i : g.get_in_neighbors()) {
		std::cout << i;
	}
	std::cout << "\n";

    return 0;
}

