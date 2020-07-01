/**
 * This script compares expected fairness with recommendation score
 * and fairness score. We choode as source nodes the 100 best by
 * pagerank nodes. 
 * @TODO: Maybe replace best by pagerank with random ones.
 * 
 *  1. We choose the best edge based on recommendation score or on the
 *  expected increase score and we test which brings the greatest gain.
 * 
 *  2. We also choose the best edge based on recommendation score or
 *  fairness and we test which has the highest expected gain.
 * 
 * We take two files:
 * 
 *  1. "expectedVsRecommendationFairness.txt": Gain score of each
 *  best edge for every source node.
 * 
 *  2. "expectedVsRecommendationExpectedFairness.txt": Expected
 *  gain for each best edge for every source node.
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

// For log purposes.
struct scores {
    double recommendationScore;
    double fairnessScore;
    double expectedScore = -1; // Means we haven't calculate it. 
};

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

static double getBestByRecFairness(std::vector<edge> edges) {
    edge candidateEdge;

    // Control initialization.
    candidateEdge.recScore = 0;

    // Find best
    for (edge e : edges) {
        if (e.recScore > candidateEdge.recScore) {
            candidateEdge = e;
        }
    }

    return candidateEdge.fairScore;
}

static double getBestByFairnessFairness(std::vector<edge> edges) {
    edge candidateEdge;

    candidateEdge.fairScore = 0;

    // Find best
    for (edge e : edges) {
        if (e.fairScore > candidateEdge.fairScore) {
            candidateEdge = e;
        }
    }

    return candidateEdge.fairScore;
}

static double getBestByExpectedFairness(std::vector<edge> edges) {
    edge candidateEdge;
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

    return candidateEdge.fairScore;
}

static double getBestByRecExp(std::vector<edge> edges) {
    edge candidateEdge;

    // Control initialization.
    candidateEdge.recScore = 0;

    // Find best
    for (edge e : edges) {
        if (e.recScore > candidateEdge.recScore) {
            candidateEdge = e;
        }
    }

    return candidateEdge.fairScore * candidateEdge.recScore;
}

static double getBestByFairnessExp(std::vector<edge> edges) {
    edge candidateEdge;

    candidateEdge.fairScore = 0;

    // Find best
    for (edge e : edges) {
        if (e.fairScore > candidateEdge.fairScore) {
            candidateEdge = e;
        }
    }

    return candidateEdge.fairScore * candidateEdge.recScore;
}

static double getBestByExpectedExp(std::vector<edge> edges) {
    edge candidateEdge;
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

    return candidateEdge.fairScore * candidateEdge.recScore;
}

static void saveVector(std::string fileName, std::vector<scores> logs) {
    std::ofstream logFile(fileName);
    logFile << "BestByRec\tBestByFair\tBestByExpected\n";
    int n = logs.size();
    for (int i = 0; i < n; i++) {
        logFile << logs[i].recommendationScore << "\t" << logs[i].fairnessScore << "\t" << logs[i].expectedScore << "\n";
    }
    logFile.close();
}

int main() {
    std::cout << "Initialize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::vector<edge> edges;
    scores newScores;
    std::vector<scores> logs;

    std::cout << "Get best by pagerank source node...\n";
    std::vector<int> sourceNodes = getBestByPagerankNodes();

    
    std::cout << "Get best by recommendation/fair/expected scores VOL 1...\n";
    for (int node : sourceNodes) {
        edges = getEdgeScores(algs, g, node);
        newScores.recommendationScore = getBestByRecFairness(edges);

        newScores.fairnessScore = getBestByFairnessFairness(edges);

        newScores.expectedScore = getBestByExpectedFairness(edges);
    
        logs.push_back(newScores);
    }
    
    saveVector("expectedVsRecommendationFairness.txt", logs);
    logs.clear();

    std::cout << "Get best by recommendation/fair/expected scores VOL 2...\n";
    for (int node : sourceNodes) {
        edges = getEdgeScores(algs, g, node);
        newScores.recommendationScore = getBestByRecExp(edges);

        newScores.fairnessScore = getBestByFairnessExp(edges);

        newScores.expectedScore = getBestByExpectedExp(edges);
    
        logs.push_back(newScores);
    }

    saveVector("expectedVsRecommendationExpectedFairness.txt", logs);

    return 0;
}
