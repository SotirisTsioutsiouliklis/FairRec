/** 
 * It adds best 5% edges by Fairness or by recommendation and computes
 * fairness and average recommendation.
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

int main() {
    std::cout << "Initailize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    edge newEdge;
    std::vector<edge> newEdges;
    std::vector<double> fairScore;
    std::vector<double> recScore;
    pagerank pagerank;
    double redPagerank;

    std::cout << "Get best by pagerank source nodes...\n";
    std::vector<int> sourceNodes = getBestByPagerankNodes();

    // Keep or 5% or 100 nodes. The smallest.
    int numberOfNodes = g.get_num_nodes();
    int numberOfEdges = (numberOfNodes / 20 < 100) ? numberOfNodes / 20 : 100;

    for (int node : sourceNodes) {
        // Get Initial red pagerank and recommendation scores.
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        fairScore.push_back(redPagerank);
        recScore.push_back(0.5);

        // Get best edges by recommendation Score.

    }



}