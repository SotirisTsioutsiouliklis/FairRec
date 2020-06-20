/**
 * Script to spot differences in greedy and fast greedy algorithms.
 * 
 * 1. Are they equivalent if not why? Where do they differs.
 * 2. How much slower is greedy?(Give percentage cause parallelization
 *    pending and probably is better measure).
 * 
 * @file greedyVsFastGreedy.cpp
 * @author Sotiris Tsioutsiouliklis.
 * @version 0.0.1 20/06/2020.
 * 
 * It is an adhock script to run in blogs network in data that has been
 * already analyzed.
*/
#include <iostream>
#include "graph.hpp"
#include "pagerank.hpp"
#include "edgeAddition.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

int main() {
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    EdgeAddition addEdges(g, algs);

    pagerank_v initialScore = addEdges.getObjectiveValues(856);

    int edges[8] = {268, 30, 1157, 135, 370, 213, 492, 4};

    for (int i : edges) {
        g.add_edge(856, i);
    }

    pagerank_v stepScore = addEdges.getObjectiveValues(856);

    std::cout << "Initial Values: " << initialScore[4].pagerank << " , " << initialScore[283].pagerank << std::endl;
    std::cout << "Step Values: " << stepScore[4].pagerank << " , " << stepScore[283].pagerank << std::endl;

    return 0;
}