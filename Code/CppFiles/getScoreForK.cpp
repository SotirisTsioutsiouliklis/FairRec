/**
 * Computes edge scores for k source nodes.
 * 
 * @file getScoreForK.cpp
 * @author Sotiris Tsioutsiouliklis.
 * @version 0.0.1 09/06/2020.
 * 
 * It selects k nodes randomly and creates an edge score file for each
 * node. Edges in file i are all edges with source the node i that 
 * don't already exist in the network.
 * 
 * @param k (int): Number of nodes.
 * @return <node i>EdgeScores.txt (text): Filesdescribed above.
*/
#include <iostream>
#include "graph.hpp"
#include "pagerank.hpp"
#include "edge_add.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

// Handles command line arguments.
static bool getArguments(const int argc, char ** const argv, int &k) {
    if (argc != 2) goto error;
    k = std::atoi(argv[1]);

    return true;

error:
    std::cerr << "Usage:\n"
	"./getScoreForK <k (int)>";

    return false;
}

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
static std::vector<int> getRandomNodesK(const graph &g, int &k) {
    int numberOfNodes = g.get_num_nodes();
    std::vector<int> randomNodes = std::vector<int> (numberOfNodes, 0);
    
    // Initialize vector with nodes' ids.
    for (int i = 0; i < numberOfNodes; i++) {
        randomNodes[i] = i;
    }

    // Suffle nodes.
    std::random_shuffle(randomNodes.begin(), randomNodes.end() );
    // Keep k first nodes.
    randomNodes.resize(k);

    return randomNodes;
}

/**
 * @param g (graph): Graph to choose nodes.
 * @param node (int): Number of nodes to choose.
 * 
 * Creates file for the node <node> with the edge scores for the edges
 * that have source the <node> and they don't already exist in the 
 * graph.
 *  
*/
static void computeEdgeScores(const graph &g, const int &node) {
    int numberOfNodes = g.get_num_nodes();
    pagerank_v edgesScore(numberOfNodes);

    // Initialize edgesScore.
    for (int i = 0; i < numberOfNodes; i++) {
        edgesScore[i].node_id = i;
        edgesScore[i].pagerank = 0;
    }
}

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

int main(int argc, char **argv)
{
    // Command line arguments.
    int k = 0;

    // Get arguments.
    if (!getArguments(argc, argv, k)) return 1;

    // Declare/Initialize variables.
    graph g("out_graph.txt", "out_community.txt");
    int numberOFNodes = g.get_num_nodes();
    pagerank_algorithms algs(g);
    Edge_addition edgeAddMethod(g, algs);
    pagerank_v edgesScore(numberOFNodes);

    // Get k Random Source nodes.
    std::vector<int> sourceNodes = getRandomNodesK(g, k);

    // For each node.
    for (int &node : sourceNodes) {
        // Compute the edges' scores.
        edgesScore = edgeAddMethod.get_objective_val(node);
        // Save the edges' scores.
        edgeAddMethod.save_vector("out_" + std::to_string(node) + "EdgesScores.txt", edgesScore);
    }

    // Save random nodes.
    saveVector("sourceNodes.txt", sourceNodes);

    return 0;
}
