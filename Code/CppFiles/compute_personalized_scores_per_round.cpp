/**
 * 
 */
#include <iostream>
#include <map>
#include "graph.hpp"
#include "pagerank.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <omp.h>
#include <iomanip>

using namespace std;


vector<int> split_int (const string &s, char delim) {
    vector<int> result;
    stringstream ss (s);
    string item;

    while (getline(ss, item, delim)) {
        result.push_back(stoi(item));
    }

    return result;
}

static void save_pagerank(std::string filename, pagerank_v &pagerankv)
{
	std::ofstream outfile_redPers;
	outfile_redPers.open(filename);
	outfile_redPers << "Node\tRed Personalized Pagerank\n";

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
    algs.set_personalization_type(personalization_t::NO_PERSONALIZATION, 0);
    recEdge edge;
    vector<recEdge> edges;
    vector<int> info;
    string str_edge;

    // Read edges.
    ifstream source_nodes("temp_edges.txt");
    while (getline(source_nodes, str_edge)) {
        info = split_int(str_edge, ',');
        edge.source = info[0];
        edge.target = info[1];
        edges.push_back(edge);
    }

    // Add edges.
    g.add_edges(edges);

    // Get pagerank.
    pagerank_v pagerank = algs.get_red_abs_prob();
    
    // Write result.
    save_pagerank("temp_score.txt", pagerank);

    return 0;
}

