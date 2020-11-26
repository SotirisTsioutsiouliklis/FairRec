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
    cout << "Initialize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    int numberOfNodes = g.get_num_nodes();
    double redPagerank;
    pagerank_v tempEdges, pagerank;
    std::ofstream edge_file("deletion_scores_predicted.txt");
    std::ofstream edge_file_act("deletion_scores_actual.txt");
    edge_file << "Source\tTarget\tExpected Fairness\n";
    edge_file_act << "Source\tTarget\tExpected Fairness\n";

    // For every source nodes.
    for (int node = 0; node < numberOfNodes; node++) {
        // Get scores for out edges.
        tempEdges = algs.getDeletionObjectiveValues(node);
        // Log edges and scores.
        for (pagerank_t &e : tempEdges) {
            edge_file << node << "\t" << e.node_id << "\t" << e.pagerank << "\n";
            edge_file.flush();

            // Remove edge.
            g.remove_edge(node, e.node_id);
            pagerank = algs.get_pagerank();
            redPagerank = g.get_pagerank_per_community(pagerank)[1];
            edge_file_act << node << "\t" << e.node_id << "\t" << redPagerank << "\n";
            g.add_edge(node, e.node_id);
            edge_file_act.flush();
        }
    }

    // Save edge scores.
    saveVector("deletion_scores_predicted.txt", predictedEdges);
    saveVector("deletion_scores_actual.txt", actuallEdges);

    return 0;
}
