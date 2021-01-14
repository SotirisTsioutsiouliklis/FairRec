/**
 * Calculates the pagerank of a network and stores result in txt file. 
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <omp.h>
#include "graph.hpp"
#include "pagerank.hpp"

static void save_pagerank(std::string filename_prefix, pagerank_v &pagerankv)
{
	std::ofstream outfile_redPers;
	outfile_redPers.open("out_" + filename_prefix + ".csv");
	outfile_redPers << "Nodes,Pagerank\n";

	for (const auto &node : pagerankv) {
		outfile_redPers << node.node_id << "," << node.pagerank << std::endl;
	}

	outfile_redPers << std::fixed;
	outfile_redPers << std::setprecision(9);
	
	// Close file.
	outfile_redPers.close();
}

int main() {
    omp_set_num_threads(20);
	// Declare arguments arguments.
	std::cout << "Initialize objects\n";
    graph g("out_graph.txt", "out_community.txt");
	pagerank_algorithms algs(g);

	pagerank_v pagerankv = algs.get_pagerank();
    save_pagerank("pagerank", pagerankv);

    return 0;
}