/**
 * Computes edge deletion scores for source nodes.
 * 
*/
#include <iostream>
#include <map>
#include "graph.hpp"
#include "pagerank.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <omp.h>

using namespace std;

vector<int> read_source_nodes(string source_file)
{
    // Variables.
    string str_node;
    int node;
    vector<int> nodes;

    // Read nodes.
    ifstream source_nodes(source_file);
    getline(source_nodes, str_node); // Skip first line with infos.
    while (getline(source_nodes, str_node)) {
        node = stoi(str_node);
        nodes.push_back(node);
    }

    return nodes;
}

int main(int argc, char **argv)
{
    // Variables.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    pagerank_v temp_scores;

    // Parse command line arguments.
    string source_file = argv[1];

    // Read source nodes.
    vector<int> source_nodes = read_source_nodes(source_file);
    // Init out file.
    std::ofstream scores_file("deletion_scores.txt");
    scores_file << "Source,Target,Score\n";
    // For each source node.
    int progress = 0;
    int no_nodes = source_nodes.size();
    for (int node : source_nodes)
    {
        // Print progress bar.
        progress++;
        cout << (progress / (float)no_nodes) * 100 << "%\r";
        cout.flush();

        // Compute deletion scores for out neighbors.
        temp_scores = algs.getDeletionObjectiveValues(node);

        // Store/Write edges.
        for (pagerank_t node_info : temp_scores)
        {
            scores_file << node << "," << node_info.node_id << "," << node_info.pagerank << "\n";
        }
        scores_file.flush();

    }
    cout << "\n";

    return 0;
}
