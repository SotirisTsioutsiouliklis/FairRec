#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "graph.hpp"
#include "pagerank.hpp"


static std::string get_local_header(const bool is_local)
{
	if (is_local)
		return "\t|\tin_n0\t\%in_n0\tout_n0\t\%out_n0\tin_n1\t\%in_n1\tout_n1\t\%out_n1\t"
			"in_neighbors_of_community_i\t\%in_neighbors_of_community_i\t"
			"out_neighbors_of_community_i\t\%out_neighbors_of_community_i...";
	else
		return "";
}

static std::string get_local_neighbors(const bool is_local, const int node_id, graph &g)
{
	if (!is_local)
		return "";

	std::string result = "\t\t|";
	const int ncommunities = g.get_num_communities();
	const int n_in_neighbors = g.get_in_degree(node_id);
	const int n_out_neighbors = g.get_out_degree(node_id);

	for (int community = 0; community < ncommunities; ++community)
	{
		int nsame_in_neighbors = g.count_in_neighbors_with_community(node_id, community);
		int in_percentage = (n_in_neighbors == 0) ? 0 : nsame_in_neighbors / (double)n_in_neighbors * 100;
		int nsame_out_neighbors = g.count_out_neighbors_with_community(node_id, community);
		int out_percentage = (n_out_neighbors == 0) ? 0 : nsame_out_neighbors / (double)n_out_neighbors * 100;
		result.append("\t");
		result.append(std::to_string(nsame_in_neighbors));
		result.append("\t(");
		result.append(std::to_string(in_percentage));
		result.append("\%)\t");
		result.append(std::to_string(nsame_out_neighbors));
		result.append("\t(");
		result.append(std::to_string(out_percentage));
		result.append("\%)");
	}

	return result;
}

static void save_pagerank(std::string filename_prefix, pagerank_v &pagerankv)
{
	std::ofstream outfile_redPers;
	outfile_redPers.open("out_" + filename_prefix + ".txt");

	for (const auto &node : pagerankv) {
		outfile_redPers << node.pagerank << std::endl;
	}

	outfile_redPers << std::fixed;
	outfile_redPers << std::setprecision(9);
	
	// Close file.
	outfile_redPers.close();
}

static void print_preamble(std::ofstream &out_summary, graph &g)
{
	std::cout << "Number of nodes: " << g.get_num_nodes() << std::endl;
	out_summary << "Number of nodes: " << g.get_num_nodes() << std::endl;
	std::cout << "Number of edges: " << g.get_num_edges() << "\n" << std::endl;
	out_summary << "Number of edges: " << g.get_num_edges() << "\n" << std::endl;

	for (int community = 0; community < g.get_num_communities(); ++community) {
		std::cout << "Community " << community << ": " << g.get_community_size(community) <<
			" nodes (" << g.get_community_size(community) / (double)g.get_num_nodes() * 100 << "%)" << std::endl;
		out_summary << "Community " << community << ": " << g.get_community_size(community) <<
			" nodes (" << g.get_community_size(community) / (double)g.get_num_nodes() * 100 << "%)" << std::endl;
	}
}

static void print_algo_info(std::string algo_name, std::ofstream &out_summary,
		personalization_t &personalize_type, double jump_prob, int extra_info)
{
	std::string personalize_type_s = "";
	if (personalize_type == personalization_t::ATTRIBUTE_PERSONALIZATION)
	{
		personalize_type_s = "attribute personalization (";
		personalize_type_s += std::to_string(extra_info);
		personalize_type_s += ") ";
	}
	else if (personalize_type == personalization_t::NODE_PERSONALIZATION)
	{
		personalize_type_s = "node personalization (";
		personalize_type_s += std::to_string(extra_info);
		personalize_type_s += ") ";
	}

	std::cout << "\nRunning " << personalize_type_s << algo_name <<
		" with jump probability = " << jump_prob << " ..." << std::endl;
	out_summary << "\nRunning " << personalize_type_s << algo_name <<
		" with jump probability = " << jump_prob << " ..." << std::endl;
}

int main(int argc, char **argv)
{
	graph g("out_graph.txt", "out_community.txt");

	pagerank_algorithms algs(g);

	// print_algo_info("Pagerank", out_summary, personalize_type, jump_prob, personalize_extra);
	pagerank_v pagerankv = algs.get_pers_pagerank();
	save_pagerank("personlized_red", pagerankv);	
}
