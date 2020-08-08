/**
 * Computes edge fairness scores for source nodes.
 * 
 * @file getScoreForK.cpp
 * @author Sotiris Tsioutsiouliklis.
 * @version 0.0.1 09/06/2020.
 * 
 * It reads the source nodes from the files and creates an edge score
 * file for each node. Edges in file i are all edges with source node
 * i that don't already exist in the network.
 * 
 * @param k (int): Number of nodes.
 * @return <node i>EdgeScores.txt (text): Filesdescribed above.
*/
#include <iostream>
#include "graph.hpp"
#include "pagerank.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <set>
#include <omp.h>

// Reads random and best by pagerank nodes.
static std::vector<int> readSourceNodes() {
    std::set<int> sourceNodes;
    std::vector<int> vecSourceNodes;
    std::string str;
    int node;

    std::ifstream randomNodes("randomSourceNodes.txt");
    std::ifstream redPagerankNodes("redBestSourceNodes.txt");
    std::ifstream bluePagerankNodes("blueBestSourceNodes.txt");
    
    getline (randomNodes, str);
    while (getline (randomNodes, str)) {
        node = std::stoi(str);
        sourceNodes.insert(node);
    }

    getline (redPagerankNodes, str);
    while (getline (redPagerankNodes, str)) {
        node = std::stoi(str);
        sourceNodes.insert(node);
    }

    getline (bluePagerankNodes, str);
    while (getline (bluePagerankNodes, str)) {
        node = std::stoi(str);
        sourceNodes.insert(node);
    }

    for (int n : sourceNodes) {
        vecSourceNodes.push_back(n);
    }

    randomNodes.close();
    redPagerankNodes.close();
    bluePagerankNodes.close();

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
*/
int main()
{
    omp_set_num_threads(20);
    std::cout << "Gets fairness scores...\n";
    std::cout << "Initialize objects...\n";
    // Declare/Initialize variables.
    graph g("out_graph.txt", "out_community.txt");
    //int numberOFNodes = g.get_num_nodes();
    pagerank_algorithms algs(g);
    pagerank_v edgesScore;
    double redPagerank = g.get_pagerank_per_community(algs.get_pagerank() )[1];

    std::vector<int> sourceNodes = readSourceNodes();
    std::vector<int> neighbors;
    // For each node.
    std::cout << "Computes fairness scores...\n";
    for (int node : sourceNodes) {
        // Compute the edges' scores.
        edgesScore = algs.getObjectiveValues(node);
        // Convert red ratio to gain.
        for (pagerank_t &e : edgesScore) {
            e.pagerank -= redPagerank;
        }
        // Save the edges' scores.
        pagerank_algorithms::saveVector(std::to_string(node) + "edgeFairnessScores.txt", edgesScore);
    }

    return 0;
}
