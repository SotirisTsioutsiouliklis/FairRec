/**
 * In this experiment we compare greedy with fast greedy for single
 * source.
 * 
 * Greedy: Recompute the objective values every time you add an edge.
 * FastGreedy: Compute objective values only one time in the begging.
 * 
 * Target of the experiment:
 *      1. See how well fastGreedy performs compared to greedy for
 *         a single source.
 *      2. Compute how much faster is fastGreedy compared to greedy
 *         for a single source.
 * 
 * Do this for 100 random source nodes and 100 best by pagerank.
 * 
 * Out files:
 *      1. For each source node: "<node_id>_redPagerankGreedy.txt":
 *      Rise of the red pagerank as we add edge with greedy algorithm.
 *      2. For each source node: "<node_id>_redPagerankFastGreedy.txt":
 *      Rise of the red pagerank as we add edge with fast greedy
 *      algorithm.
 *      3. "greedyVsFastGreedyTiming.txt": Time difference between the
 *      two algorithms - inlcudes whole algorithms with common parts
 *      counting.
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <set>
#include <cmath>
#include "graph.hpp"
#include "pagerank.hpp"
#include <chrono>
#include <omp.h>
#include <string>
#include <sstream>

int main() {
    omp_set_num_threads(20);
    std::cout << "Greedy vs FastGreedy\n";
    std::cout << "Initialize objects...\n";
    graph g("out_graph.txt", "out_community.txt");
    pagerank_algorithms algs(g);
    std::vector<int> sourceNodes = pagerank_algorithms::getRandomSourceNodes();
    
    std::cout << "Start Greedy algorithm...\n";
    auto start = std::chrono::high_resolution_clock::now();

    // Run Greedy.
    for (int i = 0; i < 100; i++) {
        std::cout << "Greedy, node: " << i << "\n";
        algs.getGreedySingleSource(sourceNodes[i], 100);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto greedyDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Start Fast Greedy algorithm...\n";
    start = std::chrono::high_resolution_clock::now();

    // Run Fast Greedy.
    for (int i = 0; i < 100; i++) {
        std::cout << "Fast Greedy, node: " << i << "\n";
        algs.getFastGreedySingleSource(sourceNodes[i], 100);
    }

    stop = std::chrono::high_resolution_clock::now();
    auto fastGreedyreedyDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    // Write time results.
    std::ofstream fileOne("greedyVsFastGreedyTiming.txt");
    fileOne << "Greedy\tFastGreedy\t(milliseconds)\n";
    fileOne << greedyDuration.count() << "\t" << fastGreedyreedyDuration.count() << "\n";
    
    // The same but for the best by pagerank nodes.
    sourceNodes.clear();
    sourceNodes = pagerank_algorithms::getBestByPagerankNodes();

    // Run Greedy.
    for (int i = 0; i < 100; i++) {
        std::cout << "Greedy, node: " << i << ":" << sourceNodes[i] << "\n";
        algs.getGreedySingleSource(sourceNodes[i], 100);
    }

    // Run Fast Greedy.
    for (int i = 0; i < 100; i++) {
        std::cout << "Fast Greedy, node: " << i << "\n";
        algs.getFastGreedySingleSource(sourceNodes[i], 100);
    }
}