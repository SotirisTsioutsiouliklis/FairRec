#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "edgeAddition.hpp"
#include <ctime>
#include <cstdlib>
#include <chrono>

// Class constructor.
EdgeAddition::EdgeAddition(graph &g, pagerank_algorithms &algs) : g(g), algs(algs) { 
    // Nothing to do here.
}

// Public.
/**
 * A greedy algorithm to add edges to a graph in
 * order to maximize red pagerank ratio. It adds k edges to a single
 * source of the graph based on formula of the FairRec paper.
 * 
 * Creates two files:
 *      1. "<sourceNode>RedPagerankGreedy.txt": Red ratio of pagerank
 *          per edge added.
 *      2. "<sourceNode>edgesGreedy.txt": Edges that added to graph.
 * 
 * @param sourceNode (int): The source node for the edges.
 * @param numberOfEdges (int): The number of edges to add.
*/
void EdgeAddition::getGreedySingleSource(int sourceNode, int numberOfEdges) {
    // Init graph and algorithms.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    EdgeAddition addEdges(g, algs);
    pagerank_v pagerank;
    double redPagerank;
    std::vector<double> redPagerankLogs;

    // Get initial red pagerank and store it.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redPagerankLogs.push_back(redPagerank);

    // Add the edges.
    edge newEdge;
    std::vector<edge> newEdges;
    newEdge.source = sourceNode;
    for (int i = 0; i < numberOfEdges; i++) {
        std::cout << "SourceNode: " << sourceNode << "Edge: " << i << std::endl;
        newEdge.target = addEdges.getBestTargetNode(sourceNode);
        g.add_edge(newEdge.source, newEdge.target);
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        newEdges.push_back(newEdge);
        redPagerankLogs.push_back(redPagerank);
    }

    EdgeAddition::saveVector(std::to_string(sourceNode) + "redPagerankGreedy.txt", redPagerankLogs);
    EdgeAddition::saveVector(std::to_string(sourceNode) + "edgesGreedy.txt", newEdges);
}

/**
 * The "fast greedy" algorithm described in fairRec paper to add edges
 * to a graph in order to maximize red pagerank ratio. It adds k edges
 * to a single source of the graph based on formula of the FairRec
 * paper.
 *
 * Creates two files:
 *      1. "<sourceNode>RedPagerankGreedy.txt": Red ratio of pagerank per edge
 *          added.
 *      2. "<sourceNode>edgesGreedy.txt": Edges that added to graph.
 * 
 * @param sourceNode (int): The source node for the edges.
 * @param numberOfEdges (int): The number of edges to add.
*/
void EdgeAddition::getFastGreedySingleSource(int sourceNode, int numberOfEdges) {
    // Init graph and algorithms.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    EdgeAddition addEdges(g, algs);
    pagerank_v pagerank;
    double redPagerank;
    std::vector<double> redPagerankLogs;

    // Get initial red pagerank.
    pagerank = algs.get_pagerank();
    redPagerank = g.get_pagerank_per_community(pagerank)[1];
    redPagerankLogs.push_back(redPagerank);

    //Get best k targets.
    std::vector<int> targetNodes = addEdges.getBestTargetNodes(sourceNode, numberOfEdges);

    // Add the edges.
    edge newEdge;
    std::vector<edge> newEdges;
    newEdge.source = sourceNode;
    for (int i = 0; i < numberOfEdges; i++) {
        std::cout << "SourceNode: " << sourceNode << "Edge: " << i << std::endl;
        newEdge.target = targetNodes[i];
        g.add_edge(newEdge.source, newEdge.target);
        pagerank = algs.get_pagerank();
        redPagerank = g.get_pagerank_per_community(pagerank)[1];
        newEdges.push_back(newEdge);
        redPagerankLogs.push_back(redPagerank);
    }

    EdgeAddition::saveVector(std::to_string(sourceNode) + "redPagerankFastGreedy.txt", redPagerankLogs);
    EdgeAddition::saveVector(std::to_string(sourceNode) + "edgesFastGreedy.txt", newEdges);
}

// Private.
std::vector<int> EdgeAddition::getRandomSourceNodes(int n) {
    std::vector<int> sourceNodes;
    int sourceNode;
    bool isSource;
    int numberOfNodes = g.get_num_nodes();

    // Get random source nodes.
    for (int i = 0; i < n; i++) {
        do {
            // Get random int from 0 to numberOfNodes - 1.
            sourceNode = (rand() % numberOfNodes);
            // Check if already exists.
            if (std::find(sourceNodes.begin(), sourceNodes.end(), sourceNode) != sourceNodes.end()) {
                isSource = true;
            } else {
                isSource = false;
            }
        } while (isSource);
        // Add node to sources.
        sourceNodes.push_back(sourceNode);
    }

    return sourceNodes;
}

int EdgeAddition::getBestTargetNode(int sourceNode) {
    std::vector<int> sourceNeighbors;
    pagerank_t candidate;
    // Get objective value for all nodes.
    pagerank_v objectiveValues = getObjectiveValues(sourceNode);
    // Remove from comparison neighbors and source by setting
    // their value to -1.
    objectiveValues[sourceNode].pagerank = -1;
    sourceNeighbors = g.get_out_neighbors(sourceNode);
    for (auto it = sourceNeighbors.begin(); it < sourceNeighbors.end(); it++) {
        objectiveValues[*it].pagerank = -1;
    }
    // Get best target node.
    candidate.node_id = -1;
    candidate.pagerank = -1;
    // Search for the best.
    for (auto it = objectiveValues.begin(); it < objectiveValues.end(); it++) {
        if (it -> pagerank > candidate.pagerank) {
            candidate.node_id = it -> node_id;
            candidate.pagerank = it -> pagerank;
        }
    }
    return candidate.node_id;
}

std::vector<int> EdgeAddition::getBestTargetNodes(int sourceNode, int k) {
    std::vector<int> sourceNeighbors;
    std::vector<int> targetNodes(k);
    // Get objective value for all nodes.
    pagerank_v objectiveValues = getObjectiveValues(sourceNode);
    // Remove from comparison neighbors and source by setting
    // their value to -1.
    objectiveValues[sourceNode].pagerank = -1;
    sourceNeighbors = g.get_out_neighbors(sourceNode);
    for (auto it = sourceNeighbors.begin(); it < sourceNeighbors.end(); it++) {
        objectiveValues[*it].pagerank = -1;
    }
    // Sort and keep first k. Implement better.
    algs.sort_pagerank_vector(objectiveValues);
    objectiveValues.resize(k);
    // Convert to node ids (ints).
    for (int i = 0; i < k; i++) {
        targetNodes[i] = objectiveValues[i].node_id;
    }
    
    return targetNodes;
}

// Save vectors.
void EdgeAddition::saveVector(std::string fileName, pagerank_v &logVector) {
    // Declare local variables.
    int n = logVector.size();

    // Open log file.
    std::ofstream logFile(fileName);

    // Write logs to file.
    logFile << "Edge\tValue\n";
    for (int i = 0; i < n; i++) {
        logFile << logVector[i].node_id << "\t" << logVector[i].pagerank << std::endl;
    }

    // Close file.
    logFile.close();
}

void EdgeAddition::saveVector(std::string fileName, std::vector<int> &logVector) {
    // Declare Variables
    int n = logVector.size();  

    std::ofstream logFile(fileName);
    // Write nodes.
    for (int i = 0; i < n; i++) {
        logFile << logVector[i] << "\n";
    }
    // Close file.
    logFile.close();
}

void EdgeAddition::saveVector(std::string fileName, std::vector<double> &logVector) {
    // Declare Variables
    int n = logVector.size();  

    std::ofstream logFile(fileName);
    // Write nodes.
    for (int i = 0; i < n; i++) {
        logFile << logVector[i] << "\n";
    }
    // Close file.
    logFile.close();
}

void EdgeAddition::saveVector(std::string fileName, std::vector<edge> &logVector) {
    std::ofstream edge_file(fileName);
    edge_file << "Source\tTarget\tSource_score_1\tSource_score_2\tTarget_score\tEdge_Score\tSource_out_d\n";
    int n = logVector.size();
    for (int i = 0; i < n; i++) {
        edge_file << logVector[i].source << "\t" << logVector[i].target << "\t" << logVector[i].s_score_1
        << "\t" << logVector[i].s_score_2 << "\t" << logVector[i].t_score << "\t" 
        << logVector[i].e_score << "\t" << logVector[i].s_outd << "\n";
    }
    edge_file.close();
}

void EdgeAddition::saveVector(std::string fileName, std::vector<step_log> &logVector) {
    // Declare local variables.
    int n = logVector.size();

    // Open log file.
    std::ofstream logFile(fileName);

    // Write logs to file.
    logFile << "Edge\tPagerank\tPrediction\tSum_Prediction\n";
    for (int i = 0; i < n; i++) {
        logFile << i << "\t" << logVector[i].red_pagerank << "\t" << logVector[i].red_pagerank_prediction << "\t" << logVector[i].red_pagerank_generalized_prediction << std::endl;
    }

    // Close file.
    logFile.close();
}


pagerank_v EdgeAddition::getObjectiveValues(int sourceNode) {
    // Declare local variables.
    pagerank_v objectiveValues, rankVector, redAbsorbingProbs, sourceAbsorbingProbs;
    std::vector<int> neighbors;
    double redPagerank, nominatorConst, denominatorConst, objectiveNominator, objectiveDenominator;
    const double jumpProb = 0.15;
    int sourceOutDegree, neighbor;
    const int numberOfNodes = g.get_num_nodes();
    objectiveValues.resize(numberOfNodes);

    // Get source out degree.
    sourceOutDegree = g.get_out_degree(sourceNode);
    // Run pagerank.
    rankVector = algs.get_pagerank();
    // Get red pagerank.
    redPagerank = g.get_pagerank_per_community(rankVector)[1];
    // Run absoring to Red.
    redAbsorbingProbs = algs.get_red_abs_prob();
    // Run absorbing to source.
    sourceAbsorbingProbs = algs.get_node_abs_prob(sourceNode);
    // Get source neighbors.
    neighbors = g.get_out_neighbors(sourceNode);
    // Get average Red pagerank of neighbors for nominator.
    nominatorConst = 0;
    if (sourceOutDegree > 0) {
        for (int nei = 0; nei < sourceOutDegree; nei++) {
            neighbor = neighbors[nei];
            nominatorConst += redAbsorbingProbs[neighbor].pagerank;
        }
        nominatorConst *= (1 / (float)sourceOutDegree);
    } else {
        for (int nei = 0; nei < numberOfNodes; nei++) {
            neighbor = nei;
            nominatorConst += redAbsorbingProbs[neighbor].pagerank;
        }
        nominatorConst *= (1 / (float)numberOfNodes);
    }
    // Get average Source pagerank of neighbors for denominator.
    denominatorConst = 0;
    if (sourceOutDegree > 0) {
        for (int nei = 0; nei < sourceOutDegree; nei++) {
            neighbor = neighbors[nei];
            denominatorConst += sourceAbsorbingProbs[neighbor].pagerank;
        }
        denominatorConst *= (1 / (float)sourceOutDegree);
    } else {
        for (int nei = 0; nei < numberOfNodes; nei++) {
            neighbor = nei;
            denominatorConst += sourceAbsorbingProbs[neighbor].pagerank;
        }
        denominatorConst *= (1 / (float)numberOfNodes);
    }
    // Calculate the Quantity. Not just the important part but
    // all so as to have a sanity check.
    // For all nodes.
    for (int targetNode = 0; targetNode < numberOfNodes; targetNode++) {
        // Calculate nominator.
        objectiveNominator = redAbsorbingProbs[targetNode].pagerank - nominatorConst;
        objectiveNominator *= ((1 - jumpProb) / jumpProb);
        // Calculate denominator.
        objectiveDenominator = sourceAbsorbingProbs[targetNode].pagerank - denominatorConst;
        objectiveDenominator *= ((1 - jumpProb) / jumpProb);
        objectiveDenominator = sourceOutDegree + 1 - objectiveDenominator;
        objectiveValues[targetNode].node_id = targetNode;
        objectiveValues[targetNode].pagerank = redPagerank + rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
        // Theory check print.
        if (objectiveDenominator < 0) std::cout << "!!!NEGATIVE DENOMINATOR!!!\n";
    }

    return objectiveValues;
}



