/**
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


vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (stoi(item));
    }

    return result;
}


int main(int argc, char **argv)
{
    string edge_file = argv[2];
    string edge_scores_file = argv[4];

    // Variables.
    string str_edge;
    vector<int> edge;
    map<int, vector<int>> adj_list;
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    pagerank_v obj_values;
    vector<int> targets;
    int source;
    int target;

    // Read edges.
    ifstream source_nodes(edge_file);
    getline(source_nodes, str_edge);
    while getline(source_nodes, str_edge) {
        edge = split(str_edge, ",");
        source = edge[0];
        target = edge[1];
        adj_list[source].push_back(target);
    }

    // Save results.
    ofstream edge_scores(edge_scores_file);
    edge_scores << "Sources,Targets,Scores\n";
    for (map<int, vector<int>::iterator iter = adj_list.begin(); iter != adj_list.end(); ++iter) {
        source = iter->first;
        targets = iter->second;
        obj_values = algs.getObjectiveValues(source);
        for (int target : targets) {
            edge_scores << source << "," << target << "," << obj_values[target] << "\n";
        }

    }
        

    return 0;
}
