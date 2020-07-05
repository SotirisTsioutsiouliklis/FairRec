/**
 * It adds to 100 random nodes one edge by fairness value.
 * It logs red pagerank after each addition.
 * 
 * @creates file "fairnesByExpectedGain.txt"
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
#include "edgeAddition.hpp"
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

static edge getBestByExpectedGainEdge(graph &g, pagerank_algorithms &algs, EdgeAddition &helpFunc, std::vector<edge> &edges, int node) {
    edge candidateEdge;
    pagerank_v objectiveValues = helpFunc.getObjectiveValues(node);
    std::vector<int> outNeighbors = g.get_out_neighbors(node);
    pagerank_v pagerank = algs.get_pagerank();
    double redPagerank = g.get_pagerank_per_community(pagerank)[1];
    candidateEdge.source = node;

    // Remove already neighbors.
    for (int nei : outNeighbors) {
        objectiveValues[nei].pagerank = 0;
    }

    // Convert to gain.
    for (pagerank_t element : objectiveValues) {
        element.pagerank -= redPagerank;
    }

    // Change edge score to gain.
    for (edge e : edges ) {
        e.fairScore = objectiveValues[e.target].pagerank;
    }

    double expectedFairness = 0;
    double tempExpectedFairness;

    // Find best
    for (edge e : edges) {
        tempExpectedFairness = e.fairScore * e.recScore;
        if (tempExpectedFairness > expectedFairness) {
            candidateEdge = e;
            expectedFairness = tempExpectedFairness;
        }
    }
    //std::cout << candidateEdge.fairScore + redPagerank << "\n";
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
    EdgeAddition helpFunc(g, algs);

    //std::vector<int> sourceNodes = getRandomSourceNodes();
    std::vector<int> sourceNodes = getBestByPagerankNodes();
    
    // Log initial pagerank.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redPagerankLogs.push_back(redPagerank);

    // Add best edge for each node and log results.
    int i = 0;
    for (int node : sourceNodes) {
        std::cout << "Getting Edge For Node: " << ++i << ", " << node  << std::endl;
        edges.clear();
        edges = getEdgeScores(algs, g, node);
        newEdge = getBestByExpectedGainEdge(g, algs, helpFunc, edges, node);
        g.add_edge(newEdge.source, newEdge.target);
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        redPagerankLogs.push_back(redPagerank);     
        //std::cout << redPagerank << "\n";
    }

    EdgeAddition::saveVector("fairnessByExpectedGain.txt", redPagerankLogs);
}