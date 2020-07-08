/** 
 * It chooses 100 radnom and 100 best by pagerank source nodes.
 * For each source node it adds best min(5% of nodes, 100) edges by red
 * pagerank gain, recommendation score or expected red pagerank gain
 * and computes cumulative red pagerank ratio, average recommendation
 * score and cumulative expected red pagerank gain per edge added for
 * each source node.
 * 
 * For every source node it creates three files:
 * 
 *  1. "<nodeid>bestRecEdges.txt": Recommendation score and fairness
 *  for best edges by recommendation score.
 *  2. "<nodeid>bestFairEdges.txt": Recommendation score and fairness
 *  for best edges by fairness score.
 *  3. "<nodeid>bestExpectEdges.txt": Recommendation score and fairness
 *  for best edges by expected gain score.
 * 
 * ***** Requires edges' recommendation scores have been computed *****
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

// Get best by expected increase score.
static void getBestExpectEdges(pagerank_algorithms & algs, graph &g, int node, std::vector<edge> &newEdges, int numberOfEdges) {
    // Get pagerank.
    pagerank_v pagerank = algs.get_pagerank();
    double redPagerank = g.get_pagerank_per_community(pagerank)[1];
    // Clear edge vector.
    std::vector<edge> candidateEdges;
    newEdges.clear();

    double expectedFairness;
    double tempExpectedFairness;
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

    int bestEdgeIndex = 0;
    for (int i = 0; i < numberOfEdges; i++) {
        expectedFairness = 0;
        for (unsigned int j = 0; j < candidateEdges.size(); j++) {
            tempExpectedFairness = candidateEdges[j].recScore * candidateEdges[j].fairScore;
            if (expectedFairness < tempExpectedFairness) {
                expectedFairness = tempExpectedFairness;
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

// Adds edges and logs red pagerank ratio, recommendation scores and expected red pagerank ratio.
static void logEdgesEffect(graph &g, pagerank_algorithms &algs, std::vector<edge> newEdges, std::string logFileName) {
    //Define variables.
    std::vector<double> fairScore, recScore, expFairScore;
    pagerank_v pagerank;
    double redPagerank;
    
    // Get Initial red pagerank and recommendation scores.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    fairScore.push_back(redPagerank);
    recScore.push_back(0.5);

    int numberOfEdges = newEdges.size();
    double gain;
    for (int i = 0; i < numberOfEdges; i++) {
        g.add_edge(newEdges[i].source, newEdges[i].target);
        // Logs rec and fair scores.
        gain = 0;
        gain = -redPagerank;
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        gain += redPagerank;
        fairScore.push_back(redPagerank);
        recScore.push_back(newEdges[i].recScore);
        expFairScore.push_back(redPagerank + (newEdges[i].recScore * gain) );
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
    logFile << "Edge\tAverageRec\tRedPagerank\tExpRedPagerank\n";
    for (int i = 0; i < numberOfEdges + 1; i++) {
        logFile << i << "\t" << recScore[i] << "\t" << fairScore[i] << "\t" << expFairScore[i] << "\n";
    }
}

int main() {
    std::cout << "Initailize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::vector<edge> newEdges;

    std::cout << "Get best by pagerank source nodes...\n";
    std::vector<int> sourceNodes = pagerank_algorithms::getBestByPagerankNodes();

    // Keep 5% or 100 nodes. The smallest.
    int numberOfNodes = g.get_num_nodes();
    int numberOfEdges = (numberOfNodes / 20 < 100) ? numberOfNodes / 20 : 100;

    std::cout << "Get recommendation score and Fairness...\n";
    int k = 0;

    for (int node : sourceNodes) {
        std::cout << "Best by pagerank node: " << k++ << ":" << node <<"\n";
        getBestRecEdges(node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestRecEdges.txt");
        getBestFairEdges(node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestFairEdges.txt");
        getBestExpectEdges(algs, g, node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestExpectEdges.txt");
    }

    std::cout << "Get random source nodes...\n";
    sourceNodes = pagerank_algorithms::getBestByPagerankNodes();

    std::cout << "Get recommendation score and Fairness...\n";
    k = 0;
    
    for (int node : sourceNodes) {
        std::cout << "Random node: " << k++ << ":" << node <<"\n";
        getBestRecEdges(node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestRecEdges.txt");
        getBestFairEdges(node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestFairEdges.txt");
        getBestExpectEdges(algs, g, node, newEdges, numberOfEdges);
        logEdgesEffect(g, algs, newEdges, std::to_string(node) + "bestExpectEdges.txt");
    }

    return 0;
}