/**
 * Test if personalized method works properly.
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
    personalization_t personalize_type = personalization_t::NODE_PERSONALIZATION;
    int personalized_node = 20;
    int source_node = 15;
    algs.set_personalization_type(personalize_type, personalized_node);
    int numberOfNodes = g.get_num_nodes();
    double redPagerank;
    pagerank_v tempEdges, pagerank;
    std::ofstream edge_file("personalized_scores_predicted.txt");
    std::ofstream edge_file_act("personalized_scores_actual.txt");
    edge_file << "Source\tTarget\tExpected Fairness\n";
    edge_file_act << "Source\tTarget\tActual Fairness\n";

        // Get scores for out edges.
        tempEdges = algs.getObjectivePersonalizedValues(personalized_node, source_node);

        // Log edges and scores.
        for (pagerank_t &e : tempEdges) {
            // Write into file the score given by the function
            edge_file << source_node << "\t" << e.node_id << "\t" << e.pagerank << "\n";
            edge_file.flush();

            // Add edge.
            g.add_edge(source_node, e.node_id);

            // Get peagerank after edge removal
            pagerank = algs.get_pagerank();

            // Get pagerank for the red team after edge removal
            redPagerank = g.get_pagerank_per_community(pagerank)[1];

            // Write into file the actual pagerank score
            edge_file_act << source_node << "\t" << e.node_id << "\t" << redPagerank << "\n";
            edge_file_act.flush();

            // remove edge
            g.remove_edge(source_node, e.node_id);
        }
    
    edge_file.close();
    edge_file_act.close();

    return 0;
}
