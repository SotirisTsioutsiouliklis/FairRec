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

static bool getOptions(const int argc, char const ** argv, int &numberOfEdges) {
    if(argc != 3) goto error;
    if(!std::strcmp(argv[1], "-n")) {
        numberOfEdges = std::atoi(argv[2]);
    } else {
        goto error;
    }
    return true;
error:
    std::cout << "Proper use: ./singleSourceGreedy.out -n <numberOfEdges>\n"; 
    return false;
}

int main(int argc, char const **argv)
{
    // Define number of threads to use.
    omp_set_num_threads(20);

    int numberOfEdges;
    if(!getOptions(argc, argv, numberOfEdges)) return 1;

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
    int sourceNode = addEdges.getRandomSourceNodes(1)[0];

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

    addEdges.saveVector(std::to_string(sourceNode) + "RedPagerankGreedy.txt", redPagerankLogs);
    addEdges.saveVector("edgesGreedy.txt", newEdges);

    return 0;
}
