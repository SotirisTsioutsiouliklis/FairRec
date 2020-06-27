/**
 * Gets absorbing probabilities of nodes. Red personalized, Blue
 * personilized or node personilized. 
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "graph.hpp"
#include "pagerank.hpp"


static bool get_options(const int argc, char ** const argv, absorb_mode &algo_mode, int &abs_node) {
    if (argc != 2 && argc != 3) goto alt;
    if (!std::strcmp(argv[1], "-r") && argc == 2) {
        algo_mode = absorb_mode::RED;

    } else if (!std::strcmp(argv[1], "-b") && argc == 2) {
        algo_mode = absorb_mode::BLUE;

    } else if (!std::strcmp(argv[1], "-n") && argc == 3) {
        algo_mode = absorb_mode::NODE;
		abs_node = std::atoi(argv[2]);

    } else {
        goto alt;
    }
    return true;

alt:
	std::cout << "Usage: " << argv[0] << " [options]\n"
		"Options:\n"
		"-r \t\t\t Probabilities to be absorbed to Red nodes.\n"
		"-b \t\t\t Probabilities to be absorbed to blue nodes.\n"
		"-n <nod_id>\t\t\t Probabilities to be absorbed to <node_id> node.\n" << std::endl;
 
    return true;
}

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

int main(int argc, char **argv)
{
	// Declare arguments arguments.
	pagerank_v pagerankv;
    absorb_mode algo_mode = absorb_mode::NONE;
	int abs_node;

	if (!get_options(argc, argv, algo_mode, abs_node)) return 1;

	// Init graph and algorithms.
	graph g("out_graph.txt", "out_community.txt");
	pagerank_algorithms algs(g);

	switch (algo_mode)
	{
	case absorb_mode::RED:
		pagerankv = algs.get_red_abs_prob();
		save_pagerank("personilized_red", pagerankv);
		break;
	case absorb_mode::BLUE:
		pagerankv = algs.get_blue_abs_prob();
		save_pagerank("personilized_blue", pagerankv);
		break;
	case absorb_mode::NODE:
		pagerankv = algs.get_node_abs_prob(abs_node);
		save_pagerank("personilized_node_" + std::to_string(abs_node), pagerankv);
		break;
	default:
        std::cout << "Running Red Absorbing.\n";
		pagerankv = algs.get_red_abs_prob();
		save_pagerank("personilized_red", pagerankv);
        std::cout << "Running Blue Absorbing.\n";
		pagerankv = algs.get_blue_abs_prob();
		save_pagerank("personilized_blue", pagerankv);
		break;
	}
}
