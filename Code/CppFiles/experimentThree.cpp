/**
 * In this experiments we choose the 100 best by paggerank nodes as
 * source nodes. For each node we add the best edge by recommendation,
 * by fairness and by expected gain. We then compute the networks
 * pagerank for each one of the forthmentioned situations.
 * 
 * Creates three files:
 * 
 *  1. "out_byRecPagerank_pagerank.txt":
 *  2. "out_byFairPagerank_pagerank.txt":
 *  3. "out_byExpectedgainPagerank_pagerank.txt":
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

// Save in txt a pagerank vector.
static void save_pagerank(std::string filename_prefix, pagerank_v &pagerankv)
{
	std::ofstream outfile_pagerank;
	outfile_pagerank.open("out_" + filename_prefix + "_pagerank.txt");
	
    for (const auto &node : pagerankv) {
		outfile_pagerank << node.pagerank << std::endl;
	}

	outfile_pagerank.close();
}

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

// Get best edges by recommendation Score.
static edge getBestRecEdge(int node) {
    // Clear edge vector.
    std::vector<edge> candidateEdges;

    edge newEdge;
    std::string str;
    // Open file.
    std::ifstream recEdges(std::to_string(node) + "edgeScores.txt");
    std::getline(recEdges, str);
    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.recScore >> newEdge.fairScore) {
        candidateEdges.push_back(newEdge);
    }

    newEdge.recScore = 0;
    for (unsigned int j = 0; j < candidateEdges.size(); j++) {
        if (candidateEdges[j].recScore > newEdge.recScore) {
            newEdge.target = candidateEdges[j].target;
            newEdge.recScore = candidateEdges[j].recScore;
            newEdge.fairScore = candidateEdges[j].fairScore;
        }
    }

    return newEdge;
}

// Get best edges by fairness Score.
static edge getBestFairEdge(int node) {
    // Clear edge vector.
    std::vector<edge> candidateEdges;

    edge newEdge;
    std::string str;
    // Open file.
    std::ifstream recEdges(std::to_string(node) + "edgeScores.txt");
    std::getline(recEdges, str);
    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.recScore >> newEdge.fairScore) {
        candidateEdges.push_back(newEdge);
    }

    newEdge.fairScore = 0;
    for (unsigned int j = 0; j < candidateEdges.size(); j++) {
        if (candidateEdges[j].fairScore > newEdge.fairScore) {
            newEdge.target = candidateEdges[j].target;
            newEdge.recScore = candidateEdges[j].recScore;
            newEdge.fairScore = candidateEdges[j].fairScore;
        }
    }
    
    return newEdge;
}

// Get best by expected increase score.
static edge getBestExpectEdge(pagerank_algorithms & algs, graph &g, int node) {
    // Get pagerank.
    pagerank_v pagerank = algs.get_pagerank();
    double redPagerank = g.get_pagerank_per_community(pagerank)[1];
    std::vector<edge> candidateEdges;

    double expectedGain;
    double tempexpectedGain;
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

    expectedGain = 0;
    for (unsigned int j = 0; j < candidateEdges.size(); j++) {
        tempexpectedGain = candidateEdges[j].recScore * candidateEdges[j].fairScore;
        if (expectedGain < tempexpectedGain) {
            expectedGain = tempexpectedGain;
            newEdge.target = candidateEdges[j].target;
            newEdge.recScore = candidateEdges[j].recScore;
            newEdge.fairScore = candidateEdges[j].fairScore;
        }
    }
    
    return newEdge;
}

// Add a set of edges to the graph.
static void addEdges(graph &g, std::vector<edge> &newEdges) {
    int sourceNode, targetNode;

    for (edge e : newEdges) {
        sourceNode = e.source;
        targetNode = e.target;
        g.add_edge(sourceNode, targetNode);
    }
}

// Remove a set of edges from the graph.
static void removeEdges(graph &g, std::vector<edge> &newEdges) {
    int sourceNode, targetNode;

    for (edge e : newEdges) {
        sourceNode = e.source;
        targetNode = e.target;
        g.remove_edge(sourceNode, targetNode);
    }
}

int main() {
    std::cout << "Initailize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::vector<edge> newEdges;
    pagerank_v pagerank;

    std::cout << "Get best by pagerank source nodes...\n";
    std::vector<int> sourceNodes = getBestByPagerankNodes();

    std::cout << "Get best edge for each source node...\n";
    
    // Get Edges by recommendation score.
    newEdges.clear();

    for (int node : sourceNodes) {
        newEdges.push_back(getBestRecEdge(node) );
    }

    addEdges(g, newEdges);
    pagerank = algs.get_pagerank();
    save_pagerank("byRecPagerank", pagerank);
    removeEdges(g, newEdges);

    // Get Edges by fairness score.
    newEdges.clear();

    for (int node : sourceNodes) {
        newEdges.push_back(getBestFairEdge(node) );
    }

    addEdges(g, newEdges);
    pagerank = algs.get_pagerank();
    save_pagerank("byFairPagerank", pagerank);
    removeEdges(g, newEdges);

    // Get Edges by expected gain score.
    newEdges.clear();

    for (int node : sourceNodes) {
        newEdges.push_back(getBestExpectEdge(algs, g, node) );
    }

    addEdges(g, newEdges);
    pagerank = algs.get_pagerank();
    save_pagerank("byExpectedGainPagerank", pagerank);
    removeEdges(g, newEdges);

    return 1;
}