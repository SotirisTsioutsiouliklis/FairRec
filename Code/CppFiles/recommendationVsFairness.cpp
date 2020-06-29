/** 
 * It adds best 5% edges by Fairness or by recommendation and computes
 * fairness and average recommendation. Also the best by prediction
 * score.
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

// Get best edges by recommendation Score.
static void getBestRecEdges(int node, std::vector<edge> &newEdges, int numberOfEdges) {
    // Clear edge vector.
    std::vector<edge> candidateEdges;
    newEdges.clear();

    edge newEdge;
    std::string str;
    // Open file.
    std::ifstream recEdges(std::to_string(node) + "edgeScores.txt");
    std::getline(recEdges, str);
    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.recScore >> newEdge.fairScore) {
        candidateEdges.push_back(newEdge);
    }

    int bestEdgeIndex = 0;
    for (int i = 0; i < numberOfEdges; i++) {
        newEdge.recScore = 0;
        for (unsigned int j = 0; j < candidateEdges.size(); j++) {
            if (candidateEdges[j].recScore > newEdge.recScore) {
                bestEdgeIndex = j;
                newEdge.target = candidateEdges[j].target;
                newEdge.recScore = candidateEdges[j].recScore;
                newEdge.fairScore = candidateEdges[j].fairScore;
            }
        }
        newEdges.push_back(newEdge);
        candidateEdges[bestEdgeIndex].recScore = 0;
    }

}

// Get best edges by fairness Score.
static void getBestFairEdges(int node, std::vector<edge> &newEdges, int numberOfEdges) {
    // Clear edge vector.
    std::vector<edge> candidateEdges;
    newEdges.clear();

    edge newEdge;
    std::string str;
    // Open file.
    std::ifstream recEdges(std::to_string(node) + "edgeScores.txt");
    std::getline(recEdges, str);
    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.recScore >> newEdge.fairScore) {
        candidateEdges.push_back(newEdge);
    }

    int bestEdgeIndex = 0;
    for (int i = 0; i < numberOfEdges; i++) {
        newEdge.fairScore = 0;
        for (unsigned int j = 0; j < candidateEdges.size(); j++) {
            if (candidateEdges[j].fairScore > newEdge.fairScore) {
                bestEdgeIndex = j;
                newEdge.target = candidateEdges[j].target;
                newEdge.recScore = candidateEdges[j].recScore;
                newEdge.fairScore = candidateEdges[j].fairScore;
            }
        }
        newEdges.push_back(newEdge);
        candidateEdges[bestEdgeIndex].fairScore = 0;
    }

}

// Adds edges and logs fairness and recommendation scores.
static void logEdgesEffect(graph &g, pagerank_algorithms &algs, std::vector<edge> newEdges, std::string logFileName) {
    //Define variables.
    std::vector<double> fairScore;
    std::vector<double> recScore;
    pagerank_v pagerank;
    double redPagerank;
    
    // Get Initial red pagerank and recommendation scores.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    fairScore.push_back(redPagerank);
    recScore.push_back(0.5);

    int numberOfEdges = newEdges.size();
    for (int i = 0; i < numberOfEdges; i++) {
        g.add_edge(newEdges[i].source, newEdges[i].target);
        // Logs rec and fair scores.
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        fairScore.push_back(redPagerank);
        recScore.push_back(newEdges[i].recScore);
    }

    // Remove new edges.
    for (int i = 0; i < numberOfEdges; i++) {
        g.remove_edge(newEdges[i].source, newEdges[i].target);
    }

    // Set Initial RecScore for nice plots and interpretable results.
    recScore[0] = recScore[1];

    // Get average Rec.
    for (int i = 1; i < numberOfEdges; i++) {
        recScore[i] = (i * recScore[i-1] + recScore[i]) / float(i + 1);
    }

    // Save
    std::ofstream logFile(logFileName);
    logFile << "Edge\tAverageRec\tFair\n";
    for (int i = 0; i < numberOfEdges + 1; i++) {
        logFile << i << "\t" << recScore[i] << "\t" << fairScore[i] << "\n";
    }
}

int main() {
    std::cout << "Initailize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::vector<edge> newEdges;

    std::cout << "Get best by pagerank source nodes...\n";
    std::vector<int> sourceNodes = getBestByPagerankNodes();

    // Keep 5% or 100 nodes. The smallest.
    int numberOfNodes = g.get_num_nodes();
    int numberOfEdges = (numberOfNodes / 20 < 100) ? numberOfNodes / 20 : 100;

    sourceNodes.resize(5);
    for (int node : sourceNodes) {
        std::cout << "Load best by rec edges for node" << node << "...\n";
        getBestRecEdges(node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestRecEdges.txt");
        std::cout << "Load best by fair edges...\n";
        getBestFairEdges(node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestFairEdges.txt");
        std::cout << "Load best by expected edges...\n";
        getBestFairEdges(node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestFairEdges.txt");
    }

    return 0;
}