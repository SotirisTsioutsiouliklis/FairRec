/**
 * Computes edge deletion scores for source nodes.
 * 
 * @file getScoreForK.cpp
 * @author Sotiris Tsioutsiouliklis.
 * @version 0.0.1 09/06/2020.
 * 
 * It reads the source nodes from the files and creates an edge score
 * file for each node. Edges in file i are all edges with source node
 * i that don't already exist in the network.
 * 
 * @param k (int): Number of nodes.
 * @return <node i>EdgeScores.txt (text): Filesdescribed above.
*/
#include <iostream>
#include "graph.hpp"
#include "pagerank.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
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

    // Parse command line arguments.
    string source_file = argv[1];

    // Read source nodes.
    vector<int> source_nodes = read_source_nodes(source_file);
    // For each source node get neighbors.
    for (int node : source_nodes)
    {
        cout << node;
    }
        // Compute deletion scores.

        // Store/Write edges.

    
    return 0;
}
