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
    std::cout << "Proper use: ./singleSourceFastGreedy.out -s <source node id> -n <numberOfEdges>\n"; 
    return false;
}

int main(int argc, char const **argv)
{
    // Define number of threads to use.
    omp_set_num_threads(20);
    //srand(time(NULL));

    int numberOfEdges, sourceNode;
    if(!getOptions(argc, argv, numberOfEdges, sourceNode)) return 1;
    EdgeAddition::getFastGreedySingleSource(sourceNode, numberOfEdges);
    

    return 0;
}
