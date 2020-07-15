/**
 * In this experiment we compare the effect of adding edges by
 * recommendation score, by gain score or by expected gain score(
 * our poor approximation of it).
 * 
 * To do that we choose 20% random nodes from the network. We then
 * calculate the forthmentioned score for each candidate edge of each
 * source node. For every criterion we add the first edge for all
 * source nodes and we log the red ratio of the network. Then the
 * second for all source node and we log again the red ratio of the
 * network, we continue until we add 10 edges to all selected source
 * nodes.
 * 
 * @NOTE: The source nodes that we use are the same to the typical
 * random source nodes we use for the most of others experiments.
 * However we have the opportunity to change them by initialize the
 * seed for the random_suffle(). In this case we must take aggain the
 * rec scores for all possible edges.
 * 
 * Target of the experiment:
 *      1. See the total impact of recommenders to the red ratio of a
 *      network.
 * 
 * Out files:
 *      1. "impactByClassicRec.txt": Pending description...
 *      2. "impactByGainRec.txt": Pending description...
 *      3. "impactByExpGainRec.txt": Pending description...
 * 
 * @TODO: Make edges independent of precalculated edge scores.
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
#include <algorithm>
#include "graph.hpp"
#include "pagerank.hpp"
#include <chrono>
#include <omp.h>
#include <string>
#include <sstream>

/**
 * Returns k random nodes of graph.
 * 
 * @param g (graph): Graph to choose nodes.
 * @param k (int): Number of nodes to choose.
 * @return randomNodes (std::vector<int>): The randomly chosen nodes.
 * 
 * TODO: Improve process when k is small enough compared to
 * numberOfNodes and so we expect to have good guesses. No need for
 * initializing the whole vector in this case, just check if the guess
 * node exists in the vector.   
*/
static std::vector<int> getRandomNodes(const graph &g, int numberOfSources) {
    int numberOfNodes = g.get_num_nodes();
    std::vector<int> randomNodes = std::vector<int> (numberOfNodes, 0);
    
    // Initialize vector with nodes' ids.
    for (int i = 0; i < numberOfNodes; i++) {
        randomNodes[i] = i;
    }

    // Suffle nodes.
    std::random_shuffle(randomNodes.begin(), randomNodes.end() );
    // Keep numberOfSources first nodes.
    randomNodes.resize(numberOfSources);

    return randomNodes;
}

// Get best edges by recommendation Score.
static void getBestRecEdges(int node, std::vector<edge> &newEdges, int numberOfEdges) {
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

int main()
{
    std::cout << "Experiment Seven\nInitialize objects...\n";
    int numberOfEdges = 10;
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    int numberOfNodes = g.get_num_nodes();
    int numberOfSources = std::min(100, numberOfNodes / 5);
    std::vector<int> sources = getRandomNodes(g, numberOfSources);
    std::vector<edge> newEdges;
    std::vector<double> redRatioLog, averageRecScore;
    double recScore = 0;

    // First for classic recommender.
    std::cout << "Find Best 10 Rec edges for each node...\n";
    
    // Keep it to check if the problem is in max size. Check if you can fix it with pointers.
 
    for (int node : sources) {
        getBestRecEdges(node, newEdges, numberOfEdges);
    }

    // Get initial red ratio.
    pagerank_v pagerank = algs.get_pagerank();
    double redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redRatioLog.push_back(redPagerank);
    averageRecScore.push_back(0);

    std::cout << "Add edges to the graph...\n";
    for (int i = 0; i < numberOfEdges; i++) {
        for (int node = 0; node < numberOfSources; node++) {
            g.add_edge(newEdges[node * numberOfEdges + i].source, newEdges[node*numberOfEdges + i].target);
            recScore += newEdges[node * numberOfEdges + i].recScore;
        }
        // Log red ratio of current state.
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        redRatioLog.push_back(redPagerank);
        averageRecScore.push_back(recScore / (double)((i + 1) * numberOfEdges) );
    }
    // For nice display.
    averageRecScore[0] = averageRecScore[1];

    std::cout << "Save by rec logs...\n";
    // Save current logs.
    pagerank_algorithms::saveVector("impactByClassicRec.txt", redRatioLog);
    pagerank_algorithms::saveVector("recScoreByClassicRec.txt", averageRecScore);

    // remove edges from graph().
    for (edge e : newEdges) {
        g.remove_edge(e.source, e.target);
    }

    // Clear new edges and logs.
    newEdges.clear();
    redRatioLog.clear();
    averageRecScore.clear();
    recScore = 0;

    // Second for gain recommender.
    std::cout << "Find Best 10 gain edges for each node...\n";
    for (int node : sources) {
        getBestFairEdges(node, newEdges, numberOfEdges);
    }
    
    // Get initial red ratio.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redRatioLog.push_back(redPagerank);
    averageRecScore.push_back(0);

    std::cout << "Add edges to the graph...\n";
    for (int i = 0; i < numberOfEdges; i++) {
        for (int node = 0; node < numberOfSources; node++) {
            g.add_edge(newEdges[node * numberOfEdges + i].source, newEdges[node*numberOfEdges + i].target);
            recScore += newEdges[node * numberOfEdges + i].recScore;
        }
        // Log red ratio of current state.
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        redRatioLog.push_back(redPagerank);
        averageRecScore.push_back(recScore / (double)((i + 1) * numberOfEdges) );
    }
    // For nice display.
    averageRecScore[0] = averageRecScore[1];

    std::cout << "Save by rec logs...\n";
    // Save current logs.
    pagerank_algorithms::saveVector("impactByGainRec.txt", redRatioLog);
    pagerank_algorithms::saveVector("recScoreByGainRec.txt", averageRecScore);

    // remove edges from graph().
    for (edge e : newEdges) {
        g.remove_edge(e.source, e.target);
    }

    // Clear new edges and logs.
    newEdges.clear();
    redRatioLog.clear();
    averageRecScore.clear();
    recScore = 0;

    // Last for expected gain recommender.
    std::cout << "Find Best 10 expected gain edges for each node...\n";
    for (int node : sources) {
        getBestExpectEdges(algs, g, node, newEdges, numberOfEdges);
    }

    // Get initial red ratio.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redRatioLog.push_back(redPagerank);
    averageRecScore.push_back(0);

    std::cout << "Add edges to the graph...\n";
    for (int i = 0; i < numberOfEdges; i++) {
        for (int node = 0; node < numberOfSources; node++) {
            g.add_edge(newEdges[node * numberOfEdges + i].source, newEdges[node*numberOfEdges + i].target);
            recScore += newEdges[node * numberOfEdges + i].recScore;
        }
        // Log red ratio of current state.
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        redRatioLog.push_back(redPagerank);
        averageRecScore.push_back(recScore / (double)((i + 1) * numberOfEdges) );
    }
    // For nice display.
    averageRecScore[0] = averageRecScore[1];

    std::cout << "Save by rec logs...\n";
    // Save current logs.
    pagerank_algorithms::saveVector("impactByExpGainRec.txt", redRatioLog);
    pagerank_algorithms::saveVector("recScoreByExpGainRec.txt", averageRecScore);

    return 0;
}
