#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "graph.hpp"
#include "pagerank.hpp"



static void save_pagerank(std::string filename_prefix, pagerank_v &pagerankv)
{
	std::ofstream outfile_redPers;
	outfile_redPers.open("out_" + filename_prefix + ".txt");
	outfile_redPers << "Node\tSum of Red Pagerank\n";

	for (const auto &node : pagerankv) {
		outfile_redPers << node.node_id << "\t" << node.pagerank << std::endl;
	}

	outfile_redPers << std::fixed;
	outfile_redPers << std::setprecision(9);
	
	// Close file.
	outfile_redPers.close();
}

int main()
{
	graph g("out_graph.txt", "out_community.txt");

	pagerank_algorithms algs(g);

	// print_algo_info("Pagerank", out_summary, personalize_type, jump_prob, personalize_extra);
	pagerank_v pagerankv = algs.get_pers_pagerank();
	save_pagerank("personilized_red", pagerankv);	
}
