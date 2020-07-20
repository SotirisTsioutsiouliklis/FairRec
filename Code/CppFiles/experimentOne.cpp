/**
 * In this experiment we compare the effect of adding edges by
 * node2vec, resource allocation, Jaccard coefficient, preferencial
 * attachment and Adamic adar recommendation scores, by gain score or
 * by expected gain score(our poor approximation of it) and with random
 * recommendations.
 * 
 * @NOTE: All forthmentioned scores are calculated in the initial state
 * of the network for all the source nodes.
 * 
 * We use as source nodes 100 random source nodes.
 * @TODO: Add other policies as well.
 *  
 * To do that we choose min(0.2 * numberOFNodes, 100) random nodes from
 * the network. We then calculate the forthmentioned score for each
 * candidate edge of each source node. For every criterion we add the
 * first edge for all source nodes and we log the red ratio of the
 * network. Then the second for all source node and we log again the
 * red ratio of the network, we continue until we add 10 edges to all
 * selected source nodes.
 * 
 * ----------------------------------------------------------------
 * 
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
#include <chrono>
#include <omp.h>
#include <string>
#include <sstream>

// Read precomputed scores from file.
static void getEdgeScores(pagerank_algorithms &algs, graph &g, std::vector<recEdge> &candidateEdges) {
    // Get pagerank.
    pagerank_v pagerank = algs.get_pagerank();
    double redPagerank = g.get_pagerank_per_community(pagerank)[1];

    // Clear edge vector.
    candidateEdges.clear();
    recEdge newEdge;
    std::string str;

    // Open file.
    std::ifstream recEdges("edgesRecScores.txt");
    std::getline(recEdges, str);
    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.node2vecScore >> newEdge.resAllocScore >> newEdge.jaccCoefScore
            >> newEdge.prefAttScore >> newEdge.adamicAdarScore) {
        candidateEdges.push_back(newEdge);
    }

}
 
/**
 * Get best edges by recommendation Score in order to be added. Add
 * first number of source nodes at each epoch.
*/
static void getBestNode2vec(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    // Clear edge vector.
    newEdges.clear();
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];

    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source == currentNode) {
            edgesPerSource[orderOfNode].push_back(e);
        } else {
            currentNode = e.source;
            orderOfNode++;
            edgesPerSource[orderOfNode].push_back(e);
        }
    }

    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextNode2vecBest(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }

}

// Returs best edge by node2vec and removes it from next competition.
static recEdge getNextNode2vecBest(std::vector<recEdge> &edges) {
    // Assume first one to be the best.
    recEdge bestEdge = edges[0];
    int bestEdgeIndex = 0;
    recEdge e;
    // Find if there is a better one.
    for (unsigned int i = 0; i < edges.size(); i++) {
        e = edges[i];
        if (e.node2vecScore > bestEdge.node2vecScore) {
            bestEdge = e;
            bestEdgeIndex = i;
        }
    }
    // Remove from the next call.
    edges[bestEdgeIndex].node2vecScore = 0; 

    return bestEdge;
}

// Get best edges by fairness Score.
static void getBestFairEdges(int node, std::vector<edge> &newEdges, int numberOfEdges) {
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

//-------------------------------------------------------------------------------------------------
/**
 * For log purposes.
 * Recommendation score depends on the recommendation algorithm.
*/
struct scores {
    double recommendationScore;
    double fairnessScore;
    double expectedScore = -1; // Means we haven't calculate it. 
};

static double getBestByRecFairness(std::vector<recEdge> edges) {
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

static double getBestByFairnessFairness(std::vector<recEdge> edges) {
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

static double getBestByExpectedFairness(std::vector<recEdge> edges) {
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

static double getBestByRecExp(std::vector<recEdge> edges) {
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

static double getBestByFairnessExp(std::vector<recEdge> edges) {
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

static double getBestByExpectedExp(std::vector<recEdge> edges) {
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
    std::vector<recEdge> candidateEdges;
    int numberOfEdges = 10; // Equivalent to number of epochs.
    int numberOfNodes = g.get_num_nodes();
    int numberOfSources = std::min(100, numberOfNodes / 5);
    std::vector<recEdge> newEdges(numberOfEdges * numberOfNodes);
    std::vector<double> redRatioLog, node2vecLog, resLog, jaccLog, prefLog, adamAdarLog;
    double node2vecScore = 0, resAllScore = 0, jaccCoefScore = 0, prefAttScore = 0, adamicAdarScore = 0;
    recEdge newEdge;

    std::cout << "Get random source nodes...\n";
    std::vector<int> sourceNodes = pagerank_algorithms::getRandomSourceNodes();

    // Read candidate edges scores.
    getEdgeScores(algs, g, candidateEdges);

    // Find 10 best by node2vec edges for each source node.
    getBestNode2vec(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.

    // Log initial metrics.
    pagerank_v pagerank = algs.get_pagerank();
    double redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redRatioLog.push_back(redPagerank);
    node2vecLog.push_back(0);
    resLog.push_back(0);
    jaccLog.push_back(0);
    prefLog.push_back(0);
    adamAdarLog.push_back(0);

    // Add edges, log metrics.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        // Add edges for one epoch.
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = newEdges[(epoch * numberOfEdges) + node];
            g.add_edge(newEdge.source, newEdge.target);
            node2vecScore += newEdge.node2vecScore;
            resAllScore += newEdge.resAllocScore;
            jaccCoefScore += newEdge.jaccCoefScore;
            prefAttScore += newEdge.prefAttScore;
            adamicAdarScore += newEdge.adamicAdarScore;
        }
        // Log metrics after one epoch.
        // Get and log red pagerank.
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        redRatioLog.push_back(redPagerank);
        // log rest.
        node2vecLog.push_back(node2vecScore / (double)( (epoch + 1 ) * numberOfSources) );
        resLog.push_back(resAllScore / (double)( (epoch + 1 ) * numberOfSources) );
        jaccLog.push_back(jaccCoefScore / (double)( (epoch + 1 ) * numberOfSources) );
        prefLog.push_back(prefAttScore / (double)( (epoch + 1 ) * numberOfSources) );
        adamAdarLog.push_back(adamicAdarScore / (double)( (epoch + 1 ) * numberOfSources) );
    }
    // For nice display.
    node2vecLog[0] = node2vecLog[1];
    resLog[0] = resLog[1];
    jaccLog[0] = jaccLog[1];
    prefLog[0] = prefLog[1];
    adamAdarLog[0] = adamAdarLog[1];

    // Save logs.
    pagerank_algorithms::saveVector("redRatioByNode2vec.txt", redRatioLog);
    pagerank_algorithms::saveVector("node2vecByNode2vec.txt", node2vecLog);
    pagerank_algorithms::saveVector("resourceAllocationByNode2vec.txt", resLog);
    pagerank_algorithms::saveVector("jaccardCoefficientByNode2vec.txt", jaccLog);
    pagerank_algorithms::saveVector("prefferentialAttachmentByNode2vec.txt", prefLog);
    pagerank_algorithms::saveVector("addamicAddarByNode2vec.txt", adamAdarLog);



    return 0;
}
