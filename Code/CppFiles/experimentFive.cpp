/**
 * It adds to 100 random nodes one edge by fairness value.
 * It logs red pagerank after each addition.
 * 
 * @creates file "fairnesByFairness.txt"
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

// Reads best by pagerank nodes.
static std::vector<int> getBestByPagerankNodes() {
    std::vector<int> sourceNodes;
    std::string str;
    int node;

    std::ifstream pagerankNodes("pagerankBestSourceNodes.txt");

    getline (pagerankNodes, str);
    while (getline (pagerankNodes, str)) {
        node = std::stoi(str);
        sourceNodes.push_back(node);
    }
    pagerankNodes.close();

    return sourceNodes;
}


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

static edge getBestByFairEdge(graph &g, pagerank_algorithms &algs, int node) {
    edge candidateEdge;
    int numberOfNodes = g.get_num_nodes();
    pagerank_v objectiveValues = algs.getObjectiveValues(node);
    std::vector<int> outNeighbors = g.get_out_neighbors(node);
    candidateEdge.source = node;

    // Remove already neighbors.
    for (int nei : outNeighbors) {
        objectiveValues[nei].pagerank = 0;
    }

    candidateEdge.fairScore = 0;
    for (int i = 0; i < numberOfNodes; i++) {
        if (objectiveValues[i].pagerank > candidateEdge.fairScore) {
            candidateEdge.target = i;
            candidateEdge.fairScore = objectiveValues[i].pagerank;
        }
    }
    //std::cout << candidateEdge.fairScore << "\n";
    return candidateEdge;
}

int main() {
    std::cout << "Initialize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    pagerank_v pagerank;
    edge newEdge;
    std::vector<double> redPagerankLogs;
    double redPagerank;

    std::vector<int> sourceNodes = getRandomSourceNodes();
    //std::vector<int> sourceNodes = getBestByPagerankNodes();
    
    // Log initial pagerank.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redPagerankLogs.push_back(redPagerank);

    // Add best edge for each node and log results.
    int i = 0;
    for (int node : sourceNodes) {
        std::cout << "Getting Edge For Node: " << ++i << ", " << node << std::endl;
        newEdge = getBestByFairEdge(g, algs, node);
        g.add_edge(newEdge.source, newEdge.target);
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        redPagerankLogs.push_back(redPagerank);
        //std::cout << redPagerank << "\n";
    }

    pagerank_algorithms::saveVector("fairnessByFairness.txt", redPagerankLogs);
}