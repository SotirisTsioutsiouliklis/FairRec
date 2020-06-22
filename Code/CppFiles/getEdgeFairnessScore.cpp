/**
 * Computes edge fairness scores for k source nodes.
 * 
 * @file getScoreForK.cpp
 * @author Sotiris Tsioutsiouliklis.
 * @version 0.0.1 09/06/2020.
 * 
 * It selects k nodes randomly and creates an edge score file for each
 * node. Edges in file i are all edges with source node i that 
 * don't already exist in the network.
 * 
 * @param k (int): Number of nodes.
 * @return <node i>EdgeScores.txt (text): Filesdescribed above.
*/
#include <iostream>
#include "graph.hpp"
#include "pagerank.hpp"
#include "edgeAddition.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <set>

// Reads random and best by pagerank nodes.
static std::vector<int> readSourceNodes() {
    std::set<int> sourceNodes;
    std::vector<int> vecSourceNodes;
    std::string str;
    int node;

    std::ifstream randomNodes("randomSourceNodes.txt");
    std::ifstream pagerankNodes("pagerankBestSourceNodes.txt");
    
    getline (randomNodes, str);
    while (getline (randomNodes, str)) {
        node = std::stoi(str);
        sourceNodes.insert(node);
    }

    getline (pagerankNodes, str);
    while (getline (pagerankNodes, str)) {
        node = std::stoi(str);
        sourceNodes.insert(node);
    }
    for (int n : sourceNodes) {
        vecSourceNodes.append(n);
    }

    randomNodes.close();
    pagerankNodes.close();

    return vecSourceNodes;
}

/**
 * @param g (graph): Graph to choose nodes.
 * @param node (int): Number of nodes to choose.
 * 
 * Creates file for the node <node> with the edge scores for the edges
 * that have source the <node> and they don't already exist in the 
 * graph.
 * 
 * TODO: Delete or fix.

static void computeEdgeScores(const graph &g, const int &node) {
    int numberOfNodes = g.get_num_nodes();
    pagerank_v edgesScore(numberOfNodes);

    // Initialize edgesScore.
    for (int i = 0; i < numberOfNodes; i++) {
        edgesScore[i].node_id = i;
        edgesScore[i].pagerank = 0;
    }
}
*/

/**
 * @param nodes (std::vector<int>): Vector of nodes' ids.
 * 
 * Create text file with the ids of the nodes  in the vector.
*/
static void saveVector(std::string fileName, std::vector<int> vec) {
    // Declare local variables.
    int numberOfNodes = vec.size();

    // Open log file.
    std::ofstream log_file(fileName);

    // Write logs to file.
    log_file << "Nodes\n";
    for (int i = 0; i < numberOfNodes; i++) {
        log_file << vec[i] << std::endl;
    }

    // Close file.
    log_file.close();
}

int main()
{
    std::cout << "Gets fairness scores...\n";
    std::cout << "Initialize objects...\n";
    // Declare/Initialize variables.
    graph g("out_graph.txt", "out_community.txt");
    int numberOFNodes = g.get_num_nodes();
    pagerank_algorithms algs(g);
    EdgeAddition edgeAddMethod(g, algs);
    pagerank_v edgesScore(numberOFNodes);

    std::vector<int> sourceNodes = readSourceNodes();

    // For each node.
    std::cout << "Computes fairness scores...\n";
    for (int &node : sourceNodes) {
        // Compute the edges' scores.
        edgesScore = edgeAddMethod.getObjectiveValues(node);
        // Save the edges' scores.
        edgeAddMethod.saveVector("out_" + std::to_string(node) + "edgeFairnessScores.txt", edgesScore);
    }

    return 0;
}
