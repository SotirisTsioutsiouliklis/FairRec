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

// PageRank.
pagerank_v pagerank_algorithms::get_pagerank(const double C, const double eps, const int max_iter)
{
	// initialize
	const unsigned int nnodes = g.get_num_nodes();
	pagerank_v pagerankv(nnodes);
	unsigned int i, node;
	#pragma omp parallel for firstprivate(nnodes)
	for (i = 0; i < nnodes; ++i) {
		pagerankv[i].node_id = i;
		pagerankv[i].pagerank = 1.0 / nnodes;
	}

	// compute pagerank for each node
	std::vector<double> tmp_pagerank(nnodes);
	std::vector<double> tmp_pagerank_jump(nnodes); // for personalization
	int iter = 0;
	for (; iter < max_iter; ++iter) {
		double sum = 0.0;
		#pragma omp parallel for firstprivate(nnodes) reduction(+:sum)
		for (node = 0; node < nnodes; ++node) {
			tmp_pagerank[node] = 0.0;
			for (const int &neighbor : g.get_in_neighbors(node)) {
				int neigh_degree = g.get_out_degree(neighbor);
				if (neigh_degree > 0)
					tmp_pagerank[node] += pagerankv[neighbor].pagerank / neigh_degree;
			}
			tmp_pagerank[node] *= C;
			sum += tmp_pagerank[node];
		}

		// re-insert "leaked" pagerank
		double diff = 0.0, new_val;
		const double leaked = (C - sum) / nnodes;
		
		compute_pagerank_no_personalization_vector(tmp_pagerank_jump, 1 - C);
		#pragma omp parallel for firstprivate(nnodes, tmp_pagerank, tmp_pagerank_jump) private(new_val) reduction(+:diff)
		for (node = 0; node < nnodes; ++node) {
			new_val = tmp_pagerank[node] + leaked + tmp_pagerank_jump[node];
			diff += std::fabs(new_val - pagerankv[node].pagerank);
			pagerankv[node].pagerank = new_val;
		}
		if (diff < eps) break;
	}

	if (iter == max_iter)
		std::cerr << "[WARN]: Pagerank algorithm reached " << max_iter << " iterations." << std::endl;
	cached_pagerank = pagerankv;
	is_cache_valid = true;
	return pagerankv;
}

// Get absoribing probabilities to red nodes.
pagerank_v pagerank_algorithms::get_red_abs_prob(const double C, const double eps, const int max_iter)
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

// Get absoribing probabilities to blue nodes.
pagerank_v pagerank_algorithms::get_blue_abs_prob(const double C, const double eps, const int max_iter)
{
	// initialize
	const unsigned int nnodes = g.get_num_nodes();
	pagerank_v pagerankv(nnodes);
	unsigned int i, node;
	for (i = 0; i < nnodes; ++i) {
		pagerankv[i].node_id = i;
		pagerankv[i].pagerank = (g.get_community(i)) ? 0 : (1-C);
	}

	// Compute Blue personilized for each node.
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
			tmp_pagerank[node] += g.get_community(node) ? 0 : (1-C);
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

// Get absorbing probabilities to node <abs_node>.
pagerank_v pagerank_algorithms::get_node_abs_prob(int abs_node, const double C, const double eps, const int max_iter) {
	// initialize
	const unsigned int nnodes = g.get_num_nodes();
	pagerank_v pagerankv(nnodes);
	unsigned int i, node;
	for (i = 0; i < nnodes; ++i) {
		pagerankv[i].node_id = i;
		pagerankv[i].pagerank = 0;
	}
	pagerankv[abs_node].pagerank = (1 - C);

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
		}
		tmp_pagerank[abs_node] += (1-C);
		
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

void pagerank_algorithms::compute_pagerank_no_personalization_vector(std::vector<double> &pagerankv,
		double total_pagerank)
{
	const double new_pagerank = total_pagerank / (double)g.get_num_nodes();
	#pragma omp parallel for
	for (unsigned int node = 0; node < pagerankv.size(); ++node) {
		pagerankv[node] = new_pagerank;
	}
}

void pagerank_algorithms::sort_pagerank_vector(pagerank_v &pagerank)
{
	std::sort(pagerank.begin(), pagerank.end());
}

