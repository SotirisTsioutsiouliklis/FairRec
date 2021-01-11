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


vector<int> split_int (const string &s, char delim) {
    vector<int> result;
    stringstream ss (s);
    string item;

    while (getline(ss, item, delim)) {
        result.push_back(stoi(item));
    }

    return result;
}

int main()
{

    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    recEdge edge;
    vector<recEdge> edges;
    vector<int> info;
    string str_edge;

    // Read edges.
    ifstream source_nodes("temp_edges.txt");
    while (getline(source_nodes, str_edge)) {
        info = split_int(str_edge, ',');
        edge.source = info[0];
        edge.target = info[1];
        edges.push_back(edge);
    }

    // Add edges.
    g.add_edges(edges);

    // Get pagerank.
    pagerank_v pagerank = algs.get_pagerank();

    // Get red pagerank.
    double red_pagerank = g.get_pagerank_per_community(pagerank)[1];
    
    // Write result.
    ofstream log_file("temp_score.txt")
    log_file << red_pagerank;
    
    
    return 0;
}

