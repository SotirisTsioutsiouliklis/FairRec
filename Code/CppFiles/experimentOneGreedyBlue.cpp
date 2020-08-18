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
#include <tuple>
#include <unordered_map>

// Read precomputed scores from file.
static void getEdgeScores(std::unordered_map<int, std::unordered_map<int, std::tuple<double, double, double, double, double, double, double> > > &candidateEdges) {
    // Clear edge vector.
    candidateEdges.clear();
    recEdge newEdge;
    std::tuple<double, double, double, double, double, double, double> newValues;
    std::string str;
    pagerank_v objectiveValues;

    // Open file.
    std::ifstream recEdges("edgesScoresBlue.txt");
    std::getline(recEdges, str);
    // Read lines.
    while (recEdges >> newEdge.source >> newEdge.target >> newEdge.node2vecScore >> newEdge.resAllocScore >> newEdge.jaccCoefScore
            >> newEdge.prefAttScore >> newEdge.adamicAdarScore >> newEdge.gain >> newEdge.expGain) {
        newValues = std::make_tuple(newEdge.node2vecScore, newEdge.resAllocScore, newEdge.jaccCoefScore, newEdge.prefAttScore,
        newEdge.adamicAdarScore, newEdge.gain, newEdge.expGain);
        candidateEdges[newEdge.source][newEdge.target] = newValues;
    }

}

int main() {
    omp_set_num_threads(20);
    std::srand(std::time(nullptr));
    std::cout << "Initialize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::unordered_map<int, std::unordered_map<int, std::tuple<double, double, double, double, double, double, double> > > candidateEdges;
    double node2vecScore = 0, resAllScore = 0, jaccCoefScore = 0, prefAttScore = 0, adamicAdarScore = 0;
    std::vector<double> redRatioLog, node2vecLog, resLog, jaccLog, prefLog, adamAdarLog;
    int numberOfEdges = 10; // Equivalent to number of epochs.
    int numberOfNodes = g.get_num_nodes();
    int numberOfSources = numberOfNodes / 10;
    std::vector<recEdge> newEdges;
    pagerank_v objectiveValues;
    std::vector<int> outNeighbors;
    recEdge newEdge;

    // Read candidate edges scores.
    std::cout << "Read edges scores...\n";
    getEdgeScores(candidateEdges);

    // Run greedy for random sources.
    std::cout << "Get random source nodes...\n";
    std::vector<int> sourceNodes = pagerank_algorithms::getBestBlueSourceNodes();
    // Log initial metrics.
    pagerank_v pagerank = algs.get_pagerank();
    double redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redRatioLog.push_back(redPagerank);
    node2vecLog.push_back(0);
    resLog.push_back(0);
    jaccLog.push_back(0);
    prefLog.push_back(0);

    for (int epoch = 0; epoch < numberOfEdges; epoch++) {
        std::cout << "Epoch: " << epoch << "\n";
        for (int node : sourceNodes) {
            newEdge.source = node;
            objectiveValues = algs.getObjectiveValues(node);
            outNeighbors = g.get_out_neighbors(node);
            objectiveValues[node].pagerank = -1;
            for (int nei : outNeighbors) {
                objectiveValues[nei].pagerank = -1;
            }
            // Find best edge.
            newEdge.node2vecScore = -1;
            for (int j = 0; j < numberOfNodes; j++) {
                if (objectiveValues[j].pagerank > newEdge.node2vecScore) {
                    newEdge.node2vecScore = objectiveValues[j].pagerank;
                    newEdge.target = j;
                }
            }
            // Add edge.
            g.add_edge(newEdge.source, newEdge.target);
        
            // Log edge.
            newEdge.node2vecScore = std::get<0>(candidateEdges.at(newEdge.source).at(newEdge.target) );
            newEdge.resAllocScore = std::get<1>(candidateEdges.at(newEdge.source).at(newEdge.target) );
            newEdge.jaccCoefScore = std::get<2>(candidateEdges.at(newEdge.source).at(newEdge.target) );
            newEdge.prefAttScore = std::get<3>(candidateEdges.at(newEdge.source).at(newEdge.target) );
            newEdge.adamicAdarScore = std::get<4>(candidateEdges.at(newEdge.source).at(newEdge.target) );
            newEdge.gain = std::get<5>(candidateEdges.at(newEdge.source).at(newEdge.target) );
            newEdge.expGain = std::get<6>(candidateEdges.at(newEdge.source).at(newEdge.target) );
            newEdges.push_back(newEdge);

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
    pagerank_algorithms::saveVector("redRatioByGreedyBlueSources.txt", redRatioLog);
    pagerank_algorithms::saveVector("node2vecByGreedyBlueSources.txt", node2vecLog);
    pagerank_algorithms::saveVector("resourceAllocationByGreedyBlueSources.txt", resLog);
    pagerank_algorithms::saveVector("jaccardCoefficientByGreedyBlueSources.txt", jaccLog);
    pagerank_algorithms::saveVector("prefferentialAttachmentByGreedyBlueSources.txt", prefLog);
    pagerank_algorithms::saveVector("addamicAddarByGreedyBlueSources.txt", adamAdarLog);
    pagerank_algorithms::saveVector("finalPagerankByGreedyBlueSources.txt", pagerank);
    // Save selected edges.
    pagerank_algorithms::saveVector("edgesSelectedByGreedyBlueSources.txt", newEdges);

    return 0;
}
