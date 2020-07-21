/**
 * It selects source nodes so as to calculate their recommendation 
 * scores, etc. so as to have them ready for all kind of experiments
 * we might need.
 * 
 * Policies ofselection:
 *  1. min(0.2 * numberOFNodes, 100) random nodes.
 *  2. min(0.2 * numberOFNodes, 100) best by pagerank / out degree
 *     red nodes.
 *  3. min(0.2 * numberOFNodes, 100) best by pagerank / out degree
 *     red nodes.
 * 
 * It creates three txt files:
 *  1. "randomSourceNodes.txt"
 *  2. "redBestedSourceNodes.txt"
 *  3. "blueBestSourceNodes.txt"
 * 
 * Every file store the nodes in decreasing order.
*/
#include <iostream>
#include "graph.hpp"
#include "pagerank.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

// Handles command line arguments. Keep it in case we need it.
/*
static bool getArguments(const int argc, char ** const argv, int &numberOfSources) {
    if (argc != 2) goto error;
    numberOfSources = std::atoi(argv[1]);

    return true;

error:
    std::cerr << "Usage:\n"
	"./getSourceNodes <numberOfSources (int)>";

    return false;
}
*/

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
 * Returns k best by pagerank / out degree red nodes of graph.
 * 
 * @param g (graph): Graph to choose nodes.
 * @param k (int): Number of nodes to choose.
 * @return sourceNodes (std::vector<int>): The chosen nodes.
 *   
*/
static std::vector<int> getBestByFormulaRed(graph &g, pagerank_algorithms &algs, int &numberOfSources) {
    // Get best by pagerank sources.
    std::vector<int> sourceNodes;
    pagerank_v pagerank = algs.get_pagerank();
    for (pagerank_t &node : pagerank) {
        node.pagerank = (g.get_community(node.node_id) == 0) ? 0 : node.pagerank / g.get_out_degree(node.node_id);
    }
    algs.sort_pagerank_vector(pagerank);
    pagerank.resize(numberOfSources);
    for (int i = 0; i < numberOfSources; i++) {
        sourceNodes.push_back(pagerank[i].node_id);
    }

    return sourceNodes;
}

/**
 * Returns k best by pagerank / out degree blue nodes of graph.
 * 
 * @param g (graph): Graph to choose nodes.
 * @param k (int): Number of nodes to choose.
 * @return sourceNodes (std::vector<int>): The chosen nodes.
 *   
*/
static std::vector<int> getBestByFormulaBlue(graph &g, pagerank_algorithms &algs, int &numberOfSources) {
    // Get best by pagerank sources.
    std::vector<int> sourceNodes;
    pagerank_v pagerank = algs.get_pagerank();
    for (pagerank_t &node : pagerank) {
        node.pagerank = (g.get_community(node.node_id) == 1) ? 0 : node.pagerank / g.get_out_degree(node.node_id);
    }
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

int main()
{
    // Keep in case we need it.
    //std::cout << "Process Command line arguments\n";
    // Get arguments.
    //if (!getArguments(argc, argv, numberOfSources)) return 1;

    std::cout << "Initializing objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);

    std::cout << "Start source selection process...\n";

    // FInd number of sources.
    int numberOfNodes = g.get_num_nodes();
    int numberOfSources = std::min(numberOfNodes / 5, 100);

    std::cout << "Getting " << numberOfSources << " random sources\n";
    // Get numberOfSources Random Source nodes.
    std::vector<int> sourceNodes = getRandomNodes(g, numberOfSources);
    // Save random nodes.
    saveVector("randomSourceNodes.txt", sourceNodes);
    std::cout << "Getting " << numberOfSources << " best by formula red sources\n";
    // Get best by pagerank.
    sourceNodes = getBestByFormulaRed(g, algs, numberOfSources);
    // Save best by pagerank nodes.
    saveVector("redBestSourceNodes.txt", sourceNodes);
    std::cout << "Getting " << numberOfSources << " best by formula blue sources\n";
    // Get best by pagerank.
    sourceNodes = getBestByFormulaBlue(g, algs, numberOfSources);
    // Save best by pagerank nodes.
    saveVector("blueBestSourceNodes.txt", sourceNodes);

    return 0;
}