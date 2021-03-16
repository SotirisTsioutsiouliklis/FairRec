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


vector<int> split_int (const string &s, char delim) {
    vector<int> result;
    stringstream ss (s);
    string item;

    while (getline(ss, item, delim)) {
        result.push_back(stoi(item));
    }

    return result;
}

int main(int argc, char **argv)
{
    // Run Parameters.
    int rounds = 10;

    string edge_file = argv[2];
    string edge_scores_file = argv[4];

    // Variables.
    string str_edge;
    vector<int> edge;
    map<int, vector<int>> adj_list;
    map<int, vector<int>> edges_to_add;
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    pagerank_v obj_values;
    vector<int> targets;
    vector<int> new_nei;
    int source;
    int target;
    pagerank_t new_target;
    int position;

    // Read edges.
    ifstream source_nodes(edge_file);
    getline(source_nodes, str_edge);
    while (getline(source_nodes, str_edge)) {
        edge = split_int(str_edge, ',');
        source = edge[0];
        target = edge[1];
        adj_list[source].push_back(target);
    }

    // Get the edges for each source.
    for (map<int, vector<int>>::iterator iter = adj_list.begin(); iter != adj_list.end(); ++iter)
    {
        // for each source node empty new_nei.
        new_nei = {};
        source = iter->first;
        targets = iter->second;
        // For each round find the best new edge based on sum formula.
        for (int r = 0; r < rounds; r++) {
            // Get objective values.
            obj_values = algs.getSumObjectiveValues(source, new_nei);
            // Find best edge.
            new_target.node_id = -1;
            new_target.pagerank = -1;
            for (int target : targets)
            {
                if (obj_values[target].pagerank > new_target.pagerank) {
                    new_target.node_id = obj_values[target].node_id;
                    new_target.pagerank = obj_values[target].pagerank;
                }
            }
            // Add it to new neighbors.
            new_nei.push_back(new_target.node_id);
            // Remove it from targets.
            for (int p = 0; p < targets.size(); p++) {
                if (targets[p] == new_target.node_id) {
                    position = p;
                }
            }
            targets.erase(targets.begin() + position);
            // Repeat!
        }
        // Store the edges for the particular source node.
        edges_to_add.insert({source, new_nei});
        // Repeat!
    }

    // Now we have all the edges to add per round for all the source nodes.
    // Save results.
    ofstream edge_scores(edge_scores_file);
    edge_scores << "Sources,Targets,Scores\n";
    for (map<int, vector<int>>::iterator iter = edges_to_add.begin(); iter != edges_to_add.end(); ++iter) {
        source = iter->first;
        targets = iter->second;
        for (int t = 0; t < targets.size(); t++) {
            edge_scores << source << "," << targets[t] << "," << 100 - t << "\n";
        }

    }
    edge_scores.close();

    return 0;
}