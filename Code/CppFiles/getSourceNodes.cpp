/**
 * It selects numberOfSources source nodes.
 * 
 * 1. Random
 * 2. Best by pagerank
 * 
 * @param numberOfSources (int)
*/
#include <iostream>
#include "graph.hpp"
#include "pagerank.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

// Handles command line arguments.
static bool getArguments(const int argc, char ** const argv, int &numberOfSources) {
    if (argc != 2) goto error;
    numberOfSources = std::atoi(argv[1]);

    return true;

error:
    std::cerr << "Usage:\n"
	"./getSourceNodes <numberOfSources (int)>";

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
static std::vector<int> getRandomNodes(const graph &g, int &numberOfSources) {
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

/**
 * Returns k best by pagerank nodes of graph.
 * 
 * @param g (graph): Graph to choose nodes.
 * @param k (int): Number of nodes to choose.
 * @return sourceNodes (std::vector<int>): The chosen nodes.
 *   
*/
static std::vector<int> getBestByPagerank(pagerank_algorithms &algs, int &numberOfSources) {
    // Get best by pagerank sources.
    std::vector<int> sourceNodes;
    pagerank_v pagerank = algs.get_pagerank();
    algs.sort_pagerank_vector(pagerank);
    pagerank.resize(numberOfSources);
    for (int i = 0; i < numberOfSources; i++) {
        sourceNodes.push_back(pagerank[i].node_id);
    }

    return sourceNodes;
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
    std::cout << "Start Program\n";
    // Command line arguments.
    int numberOfSources = 0;

    std::cout << "Process Command line arguments\n";
    // Get arguments.
    if (!getArguments(argc, argv, numberOfSources)) return 1;

    std::cout << "Initializing objects\n";
    // Declare/Initialize variables.
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);

    std::cout << "Getting " << numberOfSources << " random sources\n";
    // Get numberOfSources Random Source nodes.
    std::vector<int> sourceNodes = getRandomNodes(g, numberOfSources);
    // Save random nodes.
    saveVector("randomSourceNodes.txt", sourceNodes);
    std::cout << "Getting " << numberOfSources << " best by pagerank sources\n";
    // Get best by pagerank.
    sourceNodes = getBestByPagerank(algs, numberOfSources);
    // Save best by pagerank nodes.
    saveVector("pagerankBestSourceNodes.txt", sourceNodes);

    return 0;
}