/**
 * It adds to 100 random nodes one edge by recommendation value.
 * It logs red pagerank after each addition.
 * 
 * @creates file "fairnesByRec.txt"
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

// Reads random source nodes.
static std::vector<int> getRandomSourceNodes() {
    std::vector<int> sourceNodes;
    std::string str;
    int node;

    std::ifstream pagerankNodes("randomSourceNodes.txt");

    getline (pagerankNodes, str);
    while (getline (pagerankNodes, str)) {
        node = std::stoi(str);
        sourceNodes.push_back(node);
    }
    pagerankNodes.close();

    return sourceNodes;
}

static std::vector<edge> getEdgeScores(pagerank_algorithms &algs, graph &g, int node) {
    // Get pagerank.
    pagerank_v pagerank = algs.get_pagerank();
    double redPagerank = g.get_pagerank_per_community(pagerank)[1];

    // Clear edge vector.
    std::vector<edge> candidateEdges;
    edge newEdge;
    std::string str;

    // Open file.
    std::ifstream recEdges(std::to_string(node) + "edgeScores.txt");
    std::getline(recEdges, str);
    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.recScore >> newEdge.fairScore) {
        newEdge.fairScore -= redPagerank;
        candidateEdges.push_back(newEdge);
    }

    return candidateEdges;
}

static edge getBestByRecEdge(std::vector<edge> edges) {
    edge candidateEdge;

    // Control initialization.
    candidateEdge.recScore = 0;

    // Find best
    for (edge e : edges) {
        if (e.recScore > candidateEdge.recScore) {
            candidateEdge = e;
        }
    }

    return candidateEdge;
}

int main() {
    std::cout << "Initialize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::vector<edge> edges;
    pagerank_v pagerank;
    edge newEdge;
    std::vector<double> redPagerankLogs;
    double redPagerank;

    std::vector<int> sourceNodes = getRandomSourceNodes();
    
    // Log initial pagerank.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redPagerankLogs.push_back(redPagerank);

    // Add best edge for each node and log results.
    int i = 0;
    for (int node : sourceNodes) {
        std::cout << "Getting Edge For Node: " << ++i << std::endl;
        edges.clear();
        edges = getEdgeScores(algs, g, node);
        newEdge = getBestByRecEdge(edges);
        g.add_edge(newEdge.source, newEdge.target);
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        redPagerankLogs.push_back(redPagerank);     
    }

    pagerank_algorithms::saveVector("fairnessByRec.txt", redPagerankLogs);
}
