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
    edge_file_act << "Source\tTarget\tActual Fairness\n";

    // For every source nodes.
    for (int node = 0; node < numberOfNodes; node++) {
        // Get scores for out edges.
        cout << "0\n";
        tempEdges = algs.getDeletionObjectiveValues(node);

        // Log edges and scores.
        for (pagerank_t &e : tempEdges) {
            // Write into file the score given by the function.
            edge_file << node << "\t" << e.node_id << "\t" << e.pagerank << "\n";
            edge_file.flush();

            // Remove edge.
            g.remove_edge(node, e.node_id);

            // Get peagerank after edge removal.
            cout << "1\n";
            pagerank = algs.get_pagerank();

            // Get pagerank for the red team after edge removal.
            redPagerank = g.get_pagerank_per_community(pagerank)[1];

            // Write into file the actual pagerank score.
            edge_file_act << node << "\t" << e.node_id << "\t" << redPagerank << "\n";
            edge_file_act.flush();

            // read edge.
            g.add_edge(node, e.node_id);
        }
    }
    edge_file.close();
    edge_file_act.close();

    return 0;
}
