/**
 * This file contains a greedy algorithm to add edges to a graph in
 * order to maximize red pagerank ratio. It adds k edges to a single
 * source of the graph based on formula of the FairRec paper.
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include "graph.hpp"
#include "pagerank.hpp"
#include "edgeAddition.hpp"
#include <chrono>
#include <omp.h> 

static bool getOptions(const int argc, char const ** argv, int &numberOfEdges, int &sourceNode) {
    if(argc != 5) goto error;
    if(!std::strcmp(argv[1], "-s") && !std::strcmp(argv[3], "-n")) {
        sourceNode = std::atoi(argv[2]);
        numberOfEdges = std::atoi(argv[4]);
    } else {
        goto error;
    }
    return true;
error:
    std::cout << "Proper use: ./singleSourceGreedy.out -s <source node id> -n <numberOfEdges>\n"; 
    return false;
}

int main(int argc, char const **argv)
{
    // Define number of threads to use.
    omp_set_num_threads(20);
    auto start =std::chrono::high_resolution_clock::now();
    //srand(time(NULL));

    int numberOfEdges, sourceNode;
    if(!getOptions(argc, argv, numberOfEdges, sourceNode)) return 1;

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

    // Get random source node.
    //int sourceNode = addEdges.getRandomSourceNodes(1)[0];

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

    EdgeAddition::saveVector(std::to_string(sourceNode) + "RedPagerankGreedy.txt", redPagerankLogs);
    EdgeAddition::saveVector(std::to_string(sourceNode) + "edgesGreedy.txt", newEdges);

    auto stop =std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Total time: " << duration.count() << " microseconds\n";

    return 0;
}
