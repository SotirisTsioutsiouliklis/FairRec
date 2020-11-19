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
 * To do that we choose min(0.1 * numberOFNodes, 100) random nodes from
 * the network. We then calculate the forthmentioned score for each
 * candidate edge of each source node. For every criterion we add the
 * first edge for all source nodes and we log the red ratio of the
 * network. Then the second for all source node and we log again the
 * red ratio of the network, we continue until we add 10 edges to all
 * selected source nodes.
 * 
 * @TODO: All getBestBy functions have common parts. No need to 
 * recalculate. Improve it. 
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

using namespace std;

enum Algorithm { Node2Vec, ResourceAloc, Jaccard, PrefAttach, Adamic, Gain, ExpectedGain, Random };

// Read precomputed scores from file.
static vector<recEdge> getEdgeScores() {
    vector<recEdge> candidateEdges;
    recEdge newEdge;
    string str;
    pagerank_v objectiveValues;

    // Open file.
    ifstream recEdges("edgesScoresRandom.txt");
    getline(recEdges, str);

    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.node2vecScore >> newEdge.resAllocScore >> newEdge.jaccCoefScore
            >> newEdge.prefAttScore >> newEdge.adamicAdarScore >> newEdge.gain >> newEdge.expGain) {
        candidateEdges.push_back(newEdge);
    }

    return candidateEdges;
}

// Returs best edge by node2vec and removes it from next competition.
static recEdge getNextNode2vecBest(vector<recEdge> &edges) {
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
 * Same for resource allocation.
*/
static recEdge getNextResAllocBest(vector<recEdge> &edges) {
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

/**
 * Same for jaccard coefficient.
*/
static recEdge getNextJacCoBest(vector<recEdge> &edges) {
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

/** 
 * Same for preferencial attachment.
*/
static recEdge getNextPrefAtBest(vector<recEdge> &edges) {
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


/** 
 * Same for Adamic Adar.
*/
static recEdge getNextAdamAdarBest(vector<recEdge> &edges) {
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


/** 
 * Same for maximum gain.
*/
static recEdge getNextGainBest(vector<recEdge> &edges) {
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

/** 
 * Same for maximum expected gain.
*/
static recEdge getNextExpGainBest(vector<recEdge> &edges) {
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

/** 
 * Same for random.
*/
static recEdge getNextRandomEdge(vector<recEdge> &edges) {
    // Assume first one to be the best.
    unsigned int range = edges.size();
    unsigned int index = rand()/((RAND_MAX + 1u)/range);// Note: 1+rand()%range is biased    
    recEdge bestEdge = edges[index];

    // Remove from the next call.
    edges.erase(edges.begin() + index); 

    return bestEdge;
}

static vector<recEdge> getBestEdges(Algorithm type, vector<recEdge> &candidateEdges, int numberOfSources, int numberOfEdges) {
    string str;
    recEdge newEdge;
    vector<recEdge> newEdges;
    vector<recEdge> edgesPerSource[numberOfSources];

    // Separate edges for each source node.
    int orderOfNode = 0;
    int currentNode = candidateEdges[0].source;
    for (recEdge e : candidateEdges) {
        if (e.source != currentNode) {
            currentNode = e.source;
            orderOfNode++;
        }
        edgesPerSource[orderOfNode].push_back(e);
    }

    // Find best edges.
    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        for (int node = 0; node < numberOfSources; node++) {
            switch (type) {
                case Node2Vec:
                    newEdge = getNextNode2vecBest(edgesPerSource[node]);
                    break;
                case Jaccard:
                    newEdge = getNextJacCoBest(edgesPerSource[node]);
                    break;
                case PrefAttach:
                    newEdge = getNextPrefAtBest(edgesPerSource[node]);
                    break;
                case Adamic:
                    newEdge = getNextAdamAdarBest(edgesPerSource[node]);
                    break;
                case Gain:
                    newEdge = getNextGainBest(edgesPerSource[node]);
                    break;
                case ExpectedGain:
                    newEdge = getNextExpGainBest(edgesPerSource[node]);
                    break;
                case ResourceAloc:
                    newEdge = getNextResAllocBest(edgesPerSource[node]);
                    break;    
                case Random:
                    newEdge = getNextRandomEdge(edgesPerSource[node]);
                    break;
                default:
                    throw "Wrong Algo type has been provided.";
            }
            newEdges.push_back(newEdge);
        }
    }

    return newEdges;
}

/**
 * 
*/
static void addEdges(pagerank_algorithms &algs, graph &g, vector<recEdge> &newEdges, int numberOfEdges, int numberOfSources, string postFix) {
    vector<double> redRatioLog, node2vecLog, resLog, jaccLog, prefLog, adamAdarLog;
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
    pagerank_algorithms::saveVector("redRatio" + postFix + "RandomSources.txt", redRatioLog);
    pagerank_algorithms::saveVector("node2vec" + postFix + "RandomSources.txt", node2vecLog);
    pagerank_algorithms::saveVector("resourceAllocation" + postFix + "RandomSources.txt", resLog);
    pagerank_algorithms::saveVector("jaccardCoefficient" + postFix + "RandomSources.txt", jaccLog);
    pagerank_algorithms::saveVector("prefferentialAttachment" + postFix + "RandomSources.txt", prefLog);
    pagerank_algorithms::saveVector("addamicAddar" + postFix + "RandomSources.txt", adamAdarLog);
    pagerank_algorithms::saveVector("finalPagerank" + postFix + "RandomSources.txt", pagerank);
    // Save selected edges.
    pagerank_algorithms::saveVector("edgesSelected" + postFix + "RandomSources.txt", newEdges);

}

int main() {
    omp_set_num_threads(20);
    Algorithm algo;

    srand(time(nullptr));
    cout << "Initialize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    vector<recEdge> candidateEdges;
    int numberOfEdges = 10; // Equivalent to number of epochs.
    int numberOfNodes = g.get_num_nodes();
    int numberOfSources = numberOfNodes / 10;
    vector<recEdge> newEdges;

    // Most probably I don't need the 2 following lines.
    //cout << "Get random source nodes...\n";
    //vector<int> sourceNodes = pagerank_algorithms::getRandomSourceNodes();

    // Read candidate edges scores.
    cout << "Read edges scores...\n";
    candidateEdges = getEdgeScores();

    auto start = chrono::high_resolution_clock::now();

    // Find 10 best by node2vec edges for each source node to be removed.
    cout << "Get best by node2vec...\n";
    newEdges = getBestEdges(Node2Vec, candidateEdges, numberOfSources, numberOfEdges);// In order to be added.

    // Add edges.
    cout << "Add node2vec edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByNode2vec");
    auto stop = chrono::high_resolution_clock::now();
    auto node2vecTime = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Remove edges.
    cout << "Remove new edges...\n";
    g.remove_edges(newEdges);
    // Clear edge list.
    newEdges.clear();

    start = chrono::high_resolution_clock::now();
    // Find 10 best by resource allocation edges for each source node.
    cout << "Get best by resource allocation...\n";
    newEdges = getBestEdges(ResourceAloc, candidateEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    cout << "Add resource allocation edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByResourceAllocation");
    stop = chrono::high_resolution_clock::now();
    auto resAlocTime = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Remove edges.
    cout << "Remove new edges...\n";
    g.remove_edges(newEdges);
    // Clear edge list.
    newEdges.clear();

    start = chrono::high_resolution_clock::now();
    // Find 10 best by jaccard coefficient edges for each source node.
    cout << "Get best by Jaccard coefficient...\n";
    newEdges = getBestEdges(Jaccard, candidateEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    cout << "Add jaccard coefficient edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByJaccardCoefficient");
    stop = chrono::high_resolution_clock::now();
    auto jacCoefTime = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Remove edges.
    cout << "Remove new edges...\n";
    g.remove_edges(newEdges);
    // Clear edge list.
    newEdges.clear();

    start = chrono::high_resolution_clock::now();
    // Find 10 best by preferential attachment edges for each source node.
    cout << "Get best by preferencial attachment...\n";
    newEdges = getBestEdges(PrefAttach, candidateEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    cout << "Add prefferencial attachment edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByPreferencialAttachment");
    stop = chrono::high_resolution_clock::now();
    auto prefAttTime = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Remove edges.
    cout << "Remove new edges...\n";
    g.remove_edges(newEdges);
    // Clear edge list.
    newEdges.clear();

    start = chrono::high_resolution_clock::now();
    // Find 10 best by Adamic Adar edges for each source node.
    cout << "Get best by Adamic Adar...\n";
    newEdges = getBestEdges(Adamic, candidateEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    cout << "Add Adamic Adar edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByAdamicAdar");
    stop = chrono::high_resolution_clock::now();
    auto adamicAdarTime = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Remove edges.
    cout << "Remove new edges...\n";
    g.remove_edges(newEdges);
    // Clear edge list.
    newEdges.clear();

    start = chrono::high_resolution_clock::now();
    // Find 10 best by maximum gain edges for each source node.
    cout << "Get best by Gain...\n";
    newEdges = getBestEdges(Gain, candidateEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    cout << "Add Gain edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByGain");
    stop = chrono::high_resolution_clock::now();
    auto gainTime = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Remove edges.
    cout << "Remove new edges...\n";
    g.remove_edges(newEdges);
    // Clear edge list.
    newEdges.clear();

    start = chrono::high_resolution_clock::now();
    // Find 10 best by maximum expected gain edges for each source node.
    cout << "Get best by Exp Gain...\n";
    newEdges = getBestEdges(ExpectedGain, candidateEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    cout << "Add Exp Gain edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByExpGain");
    stop = chrono::high_resolution_clock::now();
    auto expGainTime = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Remove edges.
    cout << "Remove new edges...\n";
    g.remove_edges(newEdges);
    // Clear edge list.
    newEdges.clear();

    start = chrono::high_resolution_clock::now();
    // Find 10 best by random proposed edges for each source node.
    cout << "Get random...\n";
    newEdges = getBestEdges(Random, candidateEdges, numberOfSources, numberOfEdges);// In order to be added.
    // Add edges.
    cout << "Add random edges...\n";
    addEdges(algs, g, newEdges, numberOfEdges, numberOfSources, "ByRandom");
    stop = chrono::high_resolution_clock::now();
    auto randomTime = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Remove edges.
    cout << "Remove new edges...\n";
    g.remove_edges(newEdges);
    // Clear edge list.
    newEdges.clear();

    //Save time logs.
    ofstream timeFile("experimentOneTiming.txt");

    timeFile << "Node@vec time: " << node2vecTime.count() << " microseconds\n"
            << "Resource allocation time: " << resAlocTime.count() << " microseconds\n"
            << "Jaccard coefficient time: " << jacCoefTime.count() << " microseconds\n"
            << "Prefferential attachment time: " << prefAttTime.count() << " microseconds\n"
            << "Adamic Addar time: " << adamicAdarTime.count() << " microseconds\n"
            << "Gain time(fast greedy): " << gainTime.count() << " microseconds\n"
            << "Expected gain time: " << expGainTime.count() << " microseconds\n"
            << "Random time: " << randomTime.count() << " microseconds\n";
        
    timeFile.close();

    return 0;
}
