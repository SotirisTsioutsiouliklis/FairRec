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

using namespace std;

int main()
{
    omp_set_num_threads(20);
    
    int source_node = 10;
    int candidate_node;
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::ofstream edge_file("testing-scores.txt");
    edge_file << "actual\tpredicted\n";

    // get addition predictions.
    pagerank_v objective_values = algs.getObjectiveValues(source_node);

    // Find node to add.
    vector<int> out_neigh = g.get_out_neighbors(10);
    for (int i = 0; i < g.get_num_nodes(); i++) {
        if (find(out_neigh.begin(), out_neigh.end(), i) != out_neigh.end()) {
            candidate_node = i;
            break;
        }
    }

    // Add edge and log result.
    g.add_edge(source_node, candidate_node);
    pagerank_v pagerank = algs.get_pagerank();
    double red_pagerank = g.get_pagerank_per_community(pagerank)[1];
    edge_file << red_pagerank << "\t" << objective_values[candidate_node] << "\n";

    // remove edge to bring graph in initial state.
    g.remove_edge(source_node, candidate_node);

    // Find node to remove.
    for (int i = 0; i < g.get_num_nodes(); i++) {
        if (g.get_out_neighbors(source_node).size == 1 ) {
            break;
        }
        if (g.get_in_neighbors(i).size <= 1) {
            continue;
        } else {
            candidate_node = i;
        }
    }

    // get objective values.
    objective_values = algs.getDeletionObjectiveValues(source_node);

    // remove the edge and log the results.
    g.remove_edge(source_node, candidate_node);
    pagerank = algs.get_pagerank();
    red_pagerank = g.get_pagerank_per_community(pagerank)[1];
    edge_file << red_pagerank << "\t" << objective_values[candidate_node] << "\n";


    return 0;
}

