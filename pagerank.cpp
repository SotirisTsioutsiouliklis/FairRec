#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <cmath>
#include <cassert>
#include <omp.h>
#include "pagerank.hpp"
#include <stdlib.h>

pagerank_algorithms::pagerank_algorithms(graph &g) : g(g), is_cache_valid(false)
{
	// nothing to do here
}

// Personilized pagerank to Red for all nodes.
pagerank_v pagerank_algorithms::get_pers_pagerank(const double C, const double eps, const int max_iter)
{
	// initialize
	const unsigned int nnodes = g.get_num_nodes();
	pagerank_v pagerankv(nnodes);
	unsigned int i, node;
	for (i = 0; i < nnodes; ++i) {
		pagerankv[i].node_id = i;
		pagerankv[i].pagerank = (g.get_community(i)) ? (1-C) : 0;
	}

	// Compute Red personilized for each node.
	std::vector<double> tmp_pagerank(nnodes);
	int iter = 0;
	for (; iter < max_iter; ++iter) {
		for (node = 0; node < nnodes; ++node) {
			tmp_pagerank[node] = 0.0;
			int n_out_degree = g.get_out_degree(node);
			if (n_out_degree > 0) {
				for (const int &neighbor : g.get_out_neighbors(node)) {
					tmp_pagerank[node] += pagerankv[neighbor].pagerank / n_out_degree;
				}
			} else {
				for (i = 0; i < nnodes; i++) {
					tmp_pagerank[node] += pagerankv[i].pagerank / nnodes;
				}
			}
			tmp_pagerank[node] *= C;
			tmp_pagerank[node] += g.get_community(node) ? (1-C) : 0;
		}
		
		// Check convergence.
		double diff = 0.0;
		for (node = 0; node < nnodes; ++node) {
			diff += std::fabs(tmp_pagerank[node] - pagerankv[node].pagerank);
			pagerankv[node].pagerank = tmp_pagerank[node];
		}

		if (diff < eps) break;
	}

	if (iter == max_iter)
		std::cerr << "[WARN]: Pagerank algorithm reached " << max_iter << " iterations." << std::endl;
	
	return pagerankv;
}

void pagerank_algorithms::sort_pagerank_vector(pagerank_v &pagerank)
{
	std::sort(pagerank.begin(), pagerank.end());
}

