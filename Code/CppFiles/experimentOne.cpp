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
 * @TODO: All getBestBy functions have common parts. No need to 
 * recalculate. Improve it. 
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
    // Clear edge vector.
    candidateEdges.clear();
    recEdge newEdge;
    std::string str;
    pagerank_v objectiveValues;

    // Open file.
    std::ifstream recEdges("edgeRecScores.txt");
    std::getline(recEdges, str);
    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.node2vecScore >> newEdge.resAllocScore >> newEdge.jaccCoefScore
            >> newEdge.prefAttScore >> newEdge.adamicAdarScore >> newEdge.gain >> newEdge.expGain) {
        candidateEdges.push_back(newEdge);
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

/**
 * Appends best edges by recommendation Score in order to be added. Add
 * first number of source nodes at each epoch.
*/
static void getBestNode2vec(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];
    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
            // If 100 means next node is best red.
            if (orderOfNode == 100) break;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }
    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextNode2vecBest(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }

}

/**
 * Same for resource allocation.
*/
static recEdge getNextResAllocBest(std::vector<recEdge> &edges) {
    // Assume first one to be the best.
    recEdge bestEdge = edges[0];
    int bestEdgeIndex = 0;
    recEdge e;
    // Find if there is a better one.
    for (unsigned int i = 0; i < edges.size(); i++) {
        e = edges[i];
        if (e.resAllocScore > bestEdge.resAllocScore) {
            bestEdge = e;
            bestEdgeIndex = i;
        }
    }
    // Remove from the next call.
    edges[bestEdgeIndex].resAllocScore = 0; 

    return bestEdge;
}

static void getBestResAlloc(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];
    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
            // If 100 means next node is best red.
            if (orderOfNode == 100) break;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }
    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextResAllocBest(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }

}

/**
 * Same for jaccard coefficient.
*/
static recEdge getNextJacCoBest(std::vector<recEdge> &edges) {
    // Assume first one to be the best.
    recEdge bestEdge = edges[0];
    int bestEdgeIndex = 0;
    recEdge e;
    // Find if there is a better one.
    for (unsigned int i = 0; i < edges.size(); i++) {
        e = edges[i];
        if (e.jaccCoefScore > bestEdge.jaccCoefScore) {
            bestEdge = e;
            bestEdgeIndex = i;
        }
    }
    // Remove from the next call.
    edges[bestEdgeIndex].jaccCoefScore = 0; 

    return bestEdge;
}

static void getBestJacCo(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];
    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
            // If 100 means next node is best red.
            if (orderOfNode == 100) break;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }
    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextJacCoBest(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }

}


/** 
 * Same for preferencial attachment.
*/
static recEdge getNextPrefAtBest(std::vector<recEdge> &edges) {
    // Assume first one to be the best.
    recEdge bestEdge = edges[0];
    int bestEdgeIndex = 0;
    recEdge e;
    // Find if there is a better one.
    for (unsigned int i = 0; i < edges.size(); i++) {
        e = edges[i];
        if (e.prefAttScore > bestEdge.prefAttScore) {
            bestEdge = e;
            bestEdgeIndex = i;
        }
    }
    // Remove from the next call.
    edges[bestEdgeIndex].prefAttScore = 0; 

    return bestEdge;
}

static void getBestPrefAt(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];
    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
            // If 100 means next node is best red.
            if (orderOfNode == 100) break;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }
    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextPrefAtBest(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }

}

/** 
 * Same for Adamic Adar.
*/
static recEdge getNextAdamAdarBest(std::vector<recEdge> &edges) {
    // Assume first one to be the best.
    recEdge bestEdge = edges[0];
    int bestEdgeIndex = 0;
    recEdge e;
    // Find if there is a better one.
    for (unsigned int i = 0; i < edges.size(); i++) {
        e = edges[i];
        if (e.adamicAdarScore > bestEdge.adamicAdarScore) {
            bestEdge = e;
            bestEdgeIndex = i;
        }
    }
    // Remove from the next call.
    edges[bestEdgeIndex].adamicAdarScore = 0; 

    return bestEdge;
}

static void getBestAdamAdar(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];
    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
            // If 100 means next node is best red.
            if (orderOfNode == 100) break;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }
    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextAdamAdarBest(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }

}


/** 
 * Same for maximum gain.
*/
static recEdge getNextGainBest(std::vector<recEdge> &edges) {
    // Assume first one to be the best.
    recEdge bestEdge = edges[0];
    int bestEdgeIndex = 0;
    recEdge e;
    // Find if there is a better one.
    for (unsigned int i = 0; i < edges.size(); i++) {
        e = edges[i];
        if (e.gain > bestEdge.gain) {
            bestEdge = e;
            bestEdgeIndex = i;
        }
    }
    // Remove from the next call.
    edges[bestEdgeIndex].gain = 0; 

    return bestEdge;
}

static void getBestGain(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];
    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
            // If 100 means next node is best red.
            if (orderOfNode == 100) break;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }
    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextGainBest(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }

}
/** 
 * Same for maximum expected gain.
*/
static recEdge getNextExpGainBest(std::vector<recEdge> &edges) {
    // Assume first one to be the best.
    recEdge bestEdge = edges[0];
    int bestEdgeIndex = 0;
    recEdge e;
    // Find if there is a better one.
    for (unsigned int i = 0; i < edges.size(); i++) {
        e = edges[i];
        if (e.expGain > bestEdge.expGain) {
            bestEdge = e;
            bestEdgeIndex = i;
        }
    }
    // Remove from the next call.
    edges[bestEdgeIndex].expGain = 0; 

    return bestEdge;
}

static void getBestExpGain(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];
    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
            // If 100 means next node is best red.
            if (orderOfNode == 100) break;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }
    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextExpGainBest(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }

}

/** 
 * Same for random.
*/
static recEdge getNextRandomEdge(std::vector<recEdge> &edges) {
    // Assume first one to be the best.
    unsigned int range = edges.size();
    std::cout << range << "!\n";
    unsigned int index = std::rand()/((RAND_MAX + 1u)/range);// Note: 1+rand()%range is biased    
    recEdge bestEdge = edges[index];

    // Remove from the next call.
    edges.erase(edges.begin() + index); 

    std::cout << ".\n";
    return bestEdge;
}

static void getRandomEdges(std::vector<recEdge> &candidateEdges, std::vector<recEdge> &newEdges, int numberOfSources, int numberOfEdges) {
    recEdge newEdge;
    std::string str;
    std::vector<recEdge> edgesPerSource[numberOfSources];
    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
            // If 100 means next node is best red.
            if (orderOfNode == 100) break;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }
    // Find rendom edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            newEdge = getNextRandomEdge(edgesPerSource[node]);
            newEdges.push_back(newEdge);
        }
    }
}

/**
 * 
*/
static void addEdges(pagerank_algorithms &algs, graph &g, std::vector<recEdge> &newEdges, int numberOfEdges, int numberOfSources, std::string postFix) {
    std::vector<double> redRatioLog, node2vecLog, resLog, jaccLog, prefLog, adamAdarLog;
    double node2vecScore = 0, resAllScore = 0, jaccCoefScore = 0, prefAttScore = 0, adamicAdarScore = 0;
    recEdge newEdge;
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
            newEdge = newEdges[(epoch * numberOfSources) + node];
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
    pagerank_algorithms::saveVector("redRatio" + postFix + ".txt", redRatioLog);
    pagerank_algorithms::saveVector("node2vec" + postFix + ".txt", node2vecLog);
    pagerank_algorithms::saveVector("resourceAllocation" + postFix + ".txt", resLog);
    pagerank_algorithms::saveVector("jaccardCoefficient" + postFix + ".txt", jaccLog);
    pagerank_algorithms::saveVector("prefferentialAttachment" + postFix + ".txt", prefLog);
    pagerank_algorithms::saveVector("addamicAddar" + postFix + ".txt", adamAdarLog);
    pagerank_algorithms::saveVector("finalPagerank" + postFix + ".txt", pagerank);

}

static void removeEdges(graph &g, std::vector<recEdge> &newEdges) {
    for (recEdge e : newEdges) {
        g.remove_edge(e.source, e.target);
    }

}

int main() {
    omp_set_num_threads(5);
    std::srand(std::time(nullptr));
    std::cout << "Initialize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::vector<recEdge> candidateEdges;
    int numberOfEdges = 10; // Equivalent to number of epochs.
    int numberOfNodes = g.get_num_nodes();
    int numberOfSources = std::min(100, numberOfNodes / 5);
    std::vector<recEdge> newEdges;

    std::cout << "Get random source nodes...\n";
    std::vector<int> sourceNodes = pagerank_algorithms::getRandomSourceNodes();

    // Read candidate edges scores.
    std::cout << "Read edges scores...\n";
    getEdgeScores(algs, g, candidateEdges);
    // Find 10 best by node2vec edges for each source node.
    std::cout << "Get best by node2vec...\n";
    getBestNode2vec(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    std::cout << "Add node2vec edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByNode2vec");
    // Remove edges.
    std::cout << "Remove new edges...\n";
    removeEdges(g, newEdges);
    // Clear edge list.
    newEdges.clear();

    // Find 10 best by resource allocation edges for each source node.
    std::cout << "Get best by resource allocation...\n";
    getBestResAlloc(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    std::cout << "Add resource allocation edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByResourceAllocation");
    // Remove edges.
    std::cout << "Remove new edges...\n";
    removeEdges(g, newEdges);
    // Clear edge list.
    newEdges.clear();

    // Find 10 best by jaccard coefficient edges for each source node.
    std::cout << "Get best by Jaccard coefficient...\n";
    getBestJacCo(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    std::cout << "Add jaccard coefficient edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByJaccardCoefficient");
    // Remove edges.
    std::cout << "Remove new edges...\n";
    removeEdges(g, newEdges);
    // Clear edge list.
    newEdges.clear();

    // Find 10 best by preferential attachment edges for each source node.
    std::cout << "Get best by preferencial attachment...\n";
    getBestPrefAt(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    std::cout << "Add prefferencial attachment edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByPreferencialAttachment");
    // Remove edges.
    std::cout << "Remove new edges...\n";
    removeEdges(g, newEdges);
    // Clear edge list.
    newEdges.clear();

    // Find 10 best by Adamic Adar edges for each source node.
    std::cout << "Get best by Adamic Adar...\n";
    getBestAdamAdar(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    std::cout << "Add Adamic Adar edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByAdamicAdar");
    // Remove edges.
    std::cout << "Remove new edges...\n";
    removeEdges(g, newEdges);
    // Clear edge list.
    newEdges.clear();

    // Find 10 best by maximum gain edges for each source node.
    std::cout << "Get best by Gain...\n";
    getBestGain(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    std::cout << "Add Gain edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByGain");
    // Remove edges.
    std::cout << "Remove new edges...\n";
    removeEdges(g, newEdges);
    // Clear edge list.
    newEdges.clear();

    // Find 10 best by maximum expected gain edges for each source node.
    std::cout << "Get best by Exp Gain...\n";
    getBestExpGain(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    std::cout << "Add Exp Gain edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByExpGain");
    // Remove edges.
    std::cout << "Remove new edges...\n";
    removeEdges(g, newEdges);
    // Clear edge list.
    newEdges.clear();
    // Find 10 best by random proposed edges for each source node.
    std::cout << "Get random...\n";
    getRandomEdges(candidateEdges, newEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    std::cout << "Add random edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByRandom");
    // Remove edges.
    std::cout << "Remove new edges...\n";
    removeEdges(g, newEdges);
    // Clear edge list.
    newEdges.clear();

    return 0;
}
