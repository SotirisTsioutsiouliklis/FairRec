#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <cassert>
#include <omp.h>
#include "pagerank.hpp"
#include <stdlib.h>

using namespace std;

vector<float> split (const string &s, char delim) {
    vector<float> result;
    stringstream ss (s);
    string item;

    while (getline(ss, item, delim)) {
        result.push_back(stof(item));
    }

    return result;
}

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
		if (personalization_type == personalization_t::NO_PERSONALIZATION)
			compute_pagerank_no_personalization_vector(tmp_pagerank_jump, 1 - C);
		else
			compute_personalization_vector(tmp_pagerank_jump, 1 - C);
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
	for (i = 0; i < nnodes; ++i)
	{
		pagerankv[i].node_id = i;
		pagerankv[i].pagerank = (g.get_community(i)) ? (1 - C) : 0;
	}

	// Compute Red personilized for each node.
	std::vector<double> tmp_pagerank(nnodes);
	int iter = 0;
	for (; iter < max_iter; ++iter)
	{
#pragma omp parallel for firstprivate(nnodes)
		for (node = 0; node < nnodes; ++node)
		{
			tmp_pagerank[node] = 0.0;
			int n_out_degree = g.get_out_degree(node);
			if (n_out_degree > 0)
			{
				for (const int &neighbor : g.get_out_neighbors(node))
				{
					tmp_pagerank[node] += pagerankv[neighbor].pagerank / n_out_degree;
				}
			}
			else
			{
				for (i = 0; i < nnodes; i++)
				{
					tmp_pagerank[node] += pagerankv[i].pagerank / nnodes;
				}
			}
			tmp_pagerank[node] *= C;
			tmp_pagerank[node] += g.get_community(node) ? (1 - C) : 0;
		}

		// Check convergence.
		double diff = 0.0;
#pragma omp parallel for firstprivate(nnodes, tmp_pagerank) reduction(+ \
																	  : diff)
		for (node = 0; node < nnodes; ++node)
		{
			diff += std::fabs(tmp_pagerank[node] - pagerankv[node].pagerank);
			pagerankv[node].pagerank = tmp_pagerank[node];
		}

		if (diff < eps)
			break;
	}

	if (iter == max_iter)
		std::cerr << "[WARN]: Red absorbing probabilities algorithm reached " << max_iter << " iterations." << std::endl;

	return pagerankv;
}

// Get absoribing probabilities to blue nodes.
pagerank_v pagerank_algorithms::get_blue_abs_prob(const double C, const double eps, const int max_iter)
{
	// initialize
	const unsigned int nnodes = g.get_num_nodes();
	pagerank_v pagerankv(nnodes);
	unsigned int i, node;
	for (i = 0; i < nnodes; ++i)
	{
		pagerankv[i].node_id = i;
		pagerankv[i].pagerank = (g.get_community(i)) ? 0 : (1 - C);
	}

	// Compute Blue personilized for each node.
	std::vector<double> tmp_pagerank(nnodes);
	int iter = 0;
	for (; iter < max_iter; ++iter)
	{
#pragma omp parallel for firstprivate(nnodes)
		for (node = 0; node < nnodes; ++node)
		{
			tmp_pagerank[node] = 0.0;
			int n_out_degree = g.get_out_degree(node);
			if (n_out_degree > 0)
			{
				for (const int &neighbor : g.get_out_neighbors(node))
				{
					tmp_pagerank[node] += pagerankv[neighbor].pagerank / n_out_degree;
				}
			}
			else
			{
				for (i = 0; i < nnodes; i++)
				{
					tmp_pagerank[node] += pagerankv[i].pagerank / nnodes;
				}
			}
			tmp_pagerank[node] *= C;
			tmp_pagerank[node] += g.get_community(node) ? 0 : (1 - C);
		}

		// Check convergence.
		double diff = 0.0;
#pragma omp parallel for firstprivate(nnodes, tmp_pagerank) reduction(+ \
																	  : diff)
		for (node = 0; node < nnodes; ++node)
		{
			diff += std::fabs(tmp_pagerank[node] - pagerankv[node].pagerank);
			pagerankv[node].pagerank = tmp_pagerank[node];
		}

		if (diff < eps)
			break;
	}

	if (iter == max_iter)
		std::cerr << "[WARN]: Blue absorbing probabilities algorithm reached " << max_iter << " iterations." << std::endl;

	return pagerankv;
}

// Get absorbing probabilities to node <abs_node>.
pagerank_v pagerank_algorithms::get_node_abs_prob(int abs_node, const double C, const double eps, const int max_iter)
{
	// initialize
	const unsigned int nnodes = g.get_num_nodes();
	pagerank_v pagerankv(nnodes);
	unsigned int i, node;
	for (i = 0; i < nnodes; ++i)
	{
		pagerankv[i].node_id = i;
		pagerankv[i].pagerank = 0;
	}
	pagerankv[abs_node].pagerank = (1 - C);

	// Compute Red personilized for each node.
	std::vector<double> tmp_pagerank(nnodes);
	int iter = 0;
	for (; iter < max_iter; ++iter)
	{
#pragma omp parallel for firstprivate(nnodes)
		for (node = 0; node < nnodes; ++node)
		{
			tmp_pagerank[node] = 0.0;
			int n_out_degree = g.get_out_degree(node);
			if (n_out_degree > 0)
			{
				for (const int &neighbor : g.get_out_neighbors(node))
				{
					tmp_pagerank[node] += pagerankv[neighbor].pagerank / n_out_degree;
				}
			}
			else
			{
				for (i = 0; i < nnodes; i++)
				{
					tmp_pagerank[node] += pagerankv[i].pagerank / nnodes;
				}
			}
			tmp_pagerank[node] *= C;
		}
		tmp_pagerank[abs_node] += (1 - C);

		// Check convergence.
		double diff = 0.0;
#pragma omp parallel for firstprivate(nnodes, tmp_pagerank) reduction(+ \
																	  : diff)
		for (node = 0; node < nnodes; ++node)
		{
			diff += std::fabs(tmp_pagerank[node] - pagerankv[node].pagerank);
			pagerankv[node].pagerank = tmp_pagerank[node];
		}

		if (diff < eps)
			break;
	}

	if (iter == max_iter)
		std::cerr << "[WARN]: Single node absorbing probabilities algorithm reached " << max_iter << " iterations." << std::endl;

	return pagerankv;
}

void pagerank_algorithms::set_personalization_type(personalization_t personalize_type, int extra_info)
{
	this->personalization_type = personalize_type;
	personalization_extra_info = extra_info;
	is_cache_valid = false;
}

void pagerank_algorithms::compute_pagerank_no_personalization_vector(std::vector<double> &pagerankv,
																	 double total_pagerank)
{
	const double new_pagerank = total_pagerank / (double)g.get_num_nodes();
#pragma omp parallel for
	for (unsigned int node = 0; node < pagerankv.size(); ++node)
	{
		pagerankv[node] = new_pagerank;
	}
}

// Convention 
pagerank_v read_red_abs()
{
	ifstream my_file("red_absorbing_probabilities.csv");
	vector<float> red_probs;
	string info;
	float score;
	pagerank_v result;
	pagerank_t node;

	// Read scores.
	getline(my_file, info);
	while (getline(my_file, info)) {
        score = split(info, ',')[1];
        red_probs.push_back(score);
	}

	// Convert to pagerank_v
	for (int i = 0; i < red_probs.size(); ++i) {
		node.node_id = i;
		node.pagerank = red_probs[i];
		result.push_back(node);
	}

	return result;
}

// Convention 
pagerank_v read_pagerank()
{
	ifstream my_file("out_pagerank.csv");
	vector<float> red_probs;
	string info;
	float score;
	pagerank_v result;
	pagerank_t node;

	// Read scores.
	getline(my_file, info);
	while (getline(my_file, info)) {
        score = split(info, ',')[1];
        red_probs.push_back(score);
	}

	// Convert to pagerank_v
	for (int i = 0; i < red_probs.size(); ++i) {
		node.node_id = i;
		node.pagerank = red_probs[i];
		result.push_back(node);
	}

	return result;
}

pagerank_v pagerank_algorithms::getSumObjectiveValues(int sourceNode, vector<int> new_neighbors)
{
	// Declare local variables.
	pagerank_v objectiveValues, rankVector, redAbsorbingProbs, sourceAbsorbingProbs;
	std::vector<int> neighbors;
	double redPagerank, nominatorConst, denominatorConst, objectiveNominator, objectiveDenominator;
	const double jumpProb = 0.15;
	int sourceOutDegree, neighbor;
	const int numberOfNodes = g.get_num_nodes();
	objectiveValues.resize(numberOfNodes);
	// Get source out degree.
	sourceOutDegree = g.get_out_degree(sourceNode);

	// Run pagerank.
	rankVector = read_pagerank();
	// Get red pagerank.
	redPagerank = g.get_pagerank_per_community(rankVector)[1];

	// Run absoring to Red.
	redAbsorbingProbs = read_red_abs();//get_red_abs_prob();
	// Run absorbing to source.
	sourceAbsorbingProbs = get_node_abs_prob(sourceNode);
	// Get source neighbors.
	neighbors = g.get_out_neighbors(sourceNode);

	// Get average Red pagerank of neighbors for nominator.
	nominatorConst = 0;

	// Get average Source pagerank of neighbors for denominator.
	denominatorConst = 0;

	if (sourceOutDegree > 0) {
		for (int nei = 0; nei < sourceOutDegree; nei++) {
			neighbor = neighbors[nei];
			nominatorConst += redAbsorbingProbs[neighbor].pagerank;
			denominatorConst += sourceAbsorbingProbs[neighbor].pagerank;
		}
		nominatorConst *= (1 / (float)sourceOutDegree);
		denominatorConst *= (1 / (float)sourceOutDegree);
	}
	else {
		for (int neighbor = 0; neighbor < numberOfNodes; neighbor++) {
			nominatorConst += redAbsorbingProbs[neighbor].pagerank;
			denominatorConst += sourceAbsorbingProbs[neighbor].pagerank;
		}
		nominatorConst *= (1 / (float)numberOfNodes);
		denominatorConst *= (1 / (float)numberOfNodes);
	}

	// Calculate the Quantity. Not just the important part but
	// all so as to have a sanity check.
	// For all nodes.
	for (int targetNode = 0; targetNode < numberOfNodes; targetNode++) {
		// Calculate nominator.
		objectiveNominator = redAbsorbingProbs[targetNode].pagerank;
		for (int nei : new_neighbors)
		{
			objectiveNominator += redAbsorbingProbs[nei].pagerank;
		}
		objectiveNominator /= (new_neighbors.size() + 1);
		objectiveNominator -= nominatorConst;
		objectiveNominator *= ((1 - jumpProb) / jumpProb);
		// Calculate denominator.
		objectiveDenominator = sourceAbsorbingProbs[targetNode].pagerank;
		for (int nei : new_neighbors)
		{
			objectiveDenominator += sourceAbsorbingProbs[nei].pagerank;
		}
		objectiveDenominator /= (new_neighbors.size() + 1);
		objectiveDenominator -= denominatorConst;
		objectiveDenominator *= ((1 - jumpProb) / jumpProb);
		objectiveDenominator = ((sourceOutDegree + new_neighbors.size() + 1) / (new_neighbors.size() + 1)) - objectiveDenominator;
		objectiveValues[targetNode].node_id = targetNode;
		// objectiveValues[targetNode].pagerank = redPagerank + rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
		// Not pagerank, just the difference.
		objectiveValues[targetNode].pagerank = rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
		// Theory check print.
		if (objectiveDenominator < 0)
			std::cout << "!!!NEGATIVE DENOMINATOR!!!\n";
	}

	//pagerank_algorithms::saveVector("objectiveValues.txt", objectiveValues);

	return objectiveValues;
}

pagerank_v pagerank_algorithms::getObjectiveValues(int sourceNode)
{
	// Declare local variables.
	pagerank_v objectiveValues, rankVector, redAbsorbingProbs, sourceAbsorbingProbs;
	std::vector<int> neighbors;
	double redPagerank, nominatorConst, denominatorConst, objectiveNominator, objectiveDenominator;
	const double jumpProb = 0.15;
	int sourceOutDegree, neighbor;
	const int numberOfNodes = g.get_num_nodes();
	objectiveValues.resize(numberOfNodes);
	// Get source out degree.
	sourceOutDegree = g.get_out_degree(sourceNode);

	// Run pagerank.
	rankVector = read_pagerank();
	// Get red pagerank.
	redPagerank = g.get_pagerank_per_community(rankVector)[1];

	// Run absoring to Red.
	redAbsorbingProbs = read_red_abs();//get_red_abs_prob();
	// Run absorbing to source.
	sourceAbsorbingProbs = get_node_abs_prob(sourceNode);
	// Get source neighbors.
	neighbors = g.get_out_neighbors(sourceNode);

	// Get average Red pagerank of neighbors for nominator.
	nominatorConst = 0;

	// Get average Source pagerank of neighbors for denominator.
	denominatorConst = 0;

	if (sourceOutDegree > 0) {
		for (int nei = 0; nei < sourceOutDegree; nei++) {
			neighbor = neighbors[nei];
			nominatorConst += redAbsorbingProbs[neighbor].pagerank;
			denominatorConst += sourceAbsorbingProbs[neighbor].pagerank;
		}
		nominatorConst *= (1 / (float)sourceOutDegree);
		denominatorConst *= (1 / (float)sourceOutDegree);
	}
	else {
		for (int neighbor = 0; neighbor < numberOfNodes; neighbor++) {
			nominatorConst += redAbsorbingProbs[neighbor].pagerank;
			denominatorConst += sourceAbsorbingProbs[neighbor].pagerank;
		}
		nominatorConst *= (1 / (float)numberOfNodes);
		denominatorConst *= (1 / (float)numberOfNodes);
	}

	// Calculate the Quantity. Not just the important part but
	// all so as to have a sanity check.
	// For all nodes.
	for (int targetNode = 0; targetNode < numberOfNodes; targetNode++) {
		// Calculate nominator.
		objectiveNominator = redAbsorbingProbs[targetNode].pagerank - nominatorConst;
		objectiveNominator *= ((1 - jumpProb) / jumpProb);
		// Calculate denominator.
		objectiveDenominator = sourceAbsorbingProbs[targetNode].pagerank - denominatorConst;
		objectiveDenominator *= ((1 - jumpProb) / jumpProb);
		objectiveDenominator = sourceOutDegree + 1 - objectiveDenominator;
		objectiveValues[targetNode].node_id = targetNode;
		// objectiveValues[targetNode].pagerank = redPagerank + rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
		// Not pagerank, just the difference.
		objectiveValues[targetNode].pagerank = rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
		// Theory check print.
		if (objectiveDenominator < 0)
			std::cout << "!!!NEGATIVE DENOMINATOR!!!\n";
	}

	//pagerank_algorithms::saveVector("objectiveValues.txt", objectiveValues);

	return objectiveValues;
}

pagerank_v pagerank_algorithms::getAproxObjectiveValues(int sourceNode)
{
	// Declare local variables.
	pagerank_v objectiveValues, rankVector, redAbsorbingProbs;
	std::vector<int> neighbors;
	double redPagerank, nominatorConst, denominatorConst, objectiveNominator, objectiveDenominator;
	const double jumpProb = 0.15;
	int sourceOutDegree, neighbor;
	const int numberOfNodes = g.get_num_nodes();
	objectiveValues.resize(numberOfNodes);
	// Get source out degree.
	sourceOutDegree = g.get_out_degree(sourceNode);

	// Run pagerank.
	rankVector = read_pagerank();
	// Get red pagerank.
	redPagerank = g.get_pagerank_per_community(rankVector)[1];

	// Run absoring to Red.
	redAbsorbingProbs = read_red_abs();//get_red_abs_prob();
	// Get source neighbors.
	neighbors = g.get_out_neighbors(sourceNode);

	// Get average Red pagerank of neighbors for nominator.
	nominatorConst = 0;

	// Get average Source pagerank of neighbors for denominator.
	denominatorConst = 0;

	if (sourceOutDegree > 0) {
		for (int nei = 0; nei < sourceOutDegree; nei++) {
			neighbor = neighbors[nei];
			nominatorConst += redAbsorbingProbs[neighbor].pagerank;
			denominatorConst += g.get_out_degree(neighbor);
		}
		nominatorConst *= (1 / (float)sourceOutDegree);
		denominatorConst *= (1 / (float)sourceOutDegree);
	}
	else {
		for (int neighbor = 0; neighbor < numberOfNodes; neighbor++) {
			nominatorConst += redAbsorbingProbs[neighbor].pagerank;
			denominatorConst += g.get_out_degree(neighbor);
		}
		nominatorConst *= (1 / (float)numberOfNodes);
		denominatorConst *= (1 / (float)numberOfNodes);
	}

	// Calculate the Quantity. Not just the important part but
	// all so as to have a sanity check.
	// For all nodes.
	for (int targetNode = 0; targetNode < numberOfNodes; targetNode++) {
		// Calculate nominator.
		objectiveNominator = redAbsorbingProbs[targetNode].pagerank - nominatorConst;
		objectiveNominator *= ((1 - jumpProb) / jumpProb);
		// Calculate denominator.
		objectiveDenominator = g.get_out_degree(targetNode) - denominatorConst;
		objectiveDenominator *= ((1 - jumpProb) / jumpProb);
		objectiveDenominator = sourceOutDegree + 1 - objectiveDenominator;
		objectiveValues[targetNode].node_id = targetNode;
		objectiveValues[targetNode].pagerank = redPagerank + rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
		// Theory check print.
		if (objectiveDenominator < 0)
			std::cout << "!!!NEGATIVE DENOMINATOR!!!\n";
	}

	//pagerank_algorithms::saveVector("objectiveValues.txt", objectiveValues);

	return objectiveValues;
}

pagerank_v pagerank_algorithms::getObjectivePersonalizedValues(int personalized_node, int sourceNode)
{
	// Declare local variables.
	pagerank_v objectiveValues, rankVector, redAbsorbingProbs, sourceAbsorbingProbs;
	std::vector<int> neighbors;
	double redPagerank, nominatorConst, denominatorConst, objectiveNominator, objectiveDenominator;
	const double jumpProb = 0.15;
	int sourceOutDegree, neighbor;
	const int numberOfNodes = g.get_num_nodes();
	objectiveValues.resize(numberOfNodes);
	// Get source out degree.
	sourceOutDegree = g.get_out_degree(sourceNode);

	// Run pagerank.
	rankVector = read_pagerank();

	// Get red pagerank.
	redPagerank = g.get_pagerank_per_community(rankVector)[1];

	// Run absoring to Red.
	redAbsorbingProbs = read_red_abs();//get_red_abs_prob();

	// Run absorbing to source.
	sourceAbsorbingProbs = get_node_abs_prob(sourceNode);

	// Get source neighbors.
	neighbors = g.get_out_neighbors(sourceNode);

	// Get average Red pagerank of neighbors for nominator.
	nominatorConst = 0;

	// Get average Source pagerank of neighbors for denominator.
	denominatorConst = 0;

	if (sourceOutDegree > 0) {
		for (int nei = 0; nei < sourceOutDegree; nei++) {
			neighbor = neighbors[nei];
			nominatorConst += redAbsorbingProbs[neighbor].pagerank;
			denominatorConst += sourceAbsorbingProbs[neighbor].pagerank;
		}
		nominatorConst *= (1 / (float)sourceOutDegree);
		denominatorConst *= (1 / (float)sourceOutDegree);
	}
	else {
		for (int neighbor = 0; neighbor < numberOfNodes; neighbor++) {
			nominatorConst += redAbsorbingProbs[neighbor].pagerank;
			denominatorConst += sourceAbsorbingProbs[neighbor].pagerank;
		}
		nominatorConst *= (1 / (float)numberOfNodes);
		denominatorConst *= (1 / (float)numberOfNodes);
	}

	// Calculate the Quantity. Not just the important part but
	// all so as to have a sanity check.
	// For all nodes.
	for (int targetNode = 0; targetNode < numberOfNodes; targetNode++) {
		// Calculate nominator.
		objectiveNominator = redAbsorbingProbs[targetNode].pagerank - nominatorConst;
		objectiveNominator *= ((1 - jumpProb) / jumpProb);
		// Calculate denominator.
		objectiveDenominator = sourceAbsorbingProbs[targetNode].pagerank - denominatorConst;
		objectiveDenominator *= ((1 - jumpProb) / jumpProb);
		objectiveDenominator = sourceOutDegree + 1 - objectiveDenominator;
		objectiveValues[targetNode].node_id = targetNode;
		// objectiveValues[targetNode].pagerank = redAbsorbingProbs[personalized_node].pagerank + sourceAbsorbingProbs[personalized_node].pagerank * (objectiveNominator / objectiveDenominator);
		// Not pagerank, just the difference.
		objectiveValues[targetNode].pagerank = sourceAbsorbingProbs[personalized_node].pagerank * (objectiveNominator / objectiveDenominator);
		// Theory check print.
		if (objectiveDenominator < 0)
			std::cout << "!!!NEGATIVE DENOMINATOR!!!\n";
	}

	//pagerank_algorithms::saveVector("objectiveValues.txt", objectiveValues);

	return objectiveValues;
}

pagerank_v pagerank_algorithms::getDeletionObjectiveValues(int sourceNode)
{
	// Declare local variables.
	pagerank_v objectiveValues, rankVector, redAbsorbingProbs, sourceAbsorbingProbs;
	vector<int> neighbors, restNeighbors;
	double redPagerank, nominatorConst, denominatorConst, objectiveNominator, objectiveDenominator;
	const double jumpProb = 0.15;
	int sourceOutDegree;

	// Get source out degree.
	sourceOutDegree = g.get_out_degree(sourceNode);

	// Run pagerank.
	rankVector = get_pagerank();
	// Get red pagerank.
	redPagerank = g.get_pagerank_per_community(rankVector)[1];
	// Run absoring to Red.
	redAbsorbingProbs = get_red_abs_prob();
	// Run absorbing to source.
	sourceAbsorbingProbs = get_node_abs_prob(sourceNode);
	// Get source neighbors.
	neighbors = g.get_out_neighbors(sourceNode);

	if (sourceOutDegree > 1) { // Required for W.C.C.
		// Calculate the Quantity. Not just the important part but
		// all so as to have a sanity check.
		pagerank_t tempNode;

		// For each out neighbor.
		for (int targetNode : neighbors) {
			if (g.get_in_neighbors(targetNode).size() == 1) {
				continue;
			}

			// Get average Red pagerank of neighbors for nominator excluding the targetNode
			nominatorConst = 0;

			// Get average Source pagerank of neighbors for denominator excluding the targetNode
			denominatorConst = 0;

			for (int nei : neighbors) {
				// if (nei == targetNode) continue;
				nominatorConst += redAbsorbingProbs[nei].pagerank;
				denominatorConst += sourceAbsorbingProbs[nei].pagerank;
			}

			nominatorConst *= (1 / (float) sourceOutDegree);
			denominatorConst *= (1 / (float) sourceOutDegree);

			// Calculate nominator.
			objectiveNominator = nominatorConst - redAbsorbingProbs[targetNode].pagerank;
			objectiveNominator *= ((1 - jumpProb) / jumpProb);

			// Calculate denominator.
			objectiveDenominator = denominatorConst - sourceAbsorbingProbs[targetNode].pagerank;
			objectiveDenominator *= ((1 - jumpProb) / jumpProb);
			objectiveDenominator = sourceOutDegree - 1 - objectiveDenominator;

			tempNode.node_id = targetNode;
			// P(R) - P(R)' = ....
			tempNode.pagerank = - rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
			objectiveValues.push_back(tempNode);
		}
	}

	return objectiveValues;
}

void pagerank_algorithms::sort_pagerank_vector(pagerank_v &pagerank)
{
	std::sort(pagerank.begin(), pagerank.end());
}

// Save vectors.
void pagerank_algorithms::saveVector(std::string fileName, pagerank_v &logVector)
{
	// Declare local variables.
	int n = logVector.size();

	// Open log file.
	std::ofstream logFile(fileName);

	// Write logs to file.
	logFile << "Edge\tValue\n";
	for (int i = 0; i < n; i++)
	{
		logFile << logVector[i].node_id << "\t" << logVector[i].pagerank << std::endl;
	}

	// Close file.
	logFile.close();
}

void pagerank_algorithms::saveVector(std::string fileName, std::vector<int> &logVector)
{
	// Declare Variables
	int n = logVector.size();

	std::ofstream logFile(fileName);
	// Write nodes.
	for (int i = 0; i < n; i++)
	{
		logFile << logVector[i] << "\n";
	}
	// Close file.
	logFile.close();
}

void pagerank_algorithms::saveVector(std::string fileName, std::vector<double> &logVector)
{
	// Declare Variables
	int n = logVector.size();

	std::ofstream logFile(fileName);
	// Write nodes.
	for (int i = 0; i < n; i++)
	{
		logFile << logVector[i] << "\n";
	}
	// Close file.
	logFile.close();
}

void pagerank_algorithms::saveVector(std::string fileName, std::vector<edge> &logVector)
{
	std::ofstream edge_file(fileName);
	edge_file << "Source\tTarget\tRecScore\tFairScore\n";
	int n = logVector.size();
	for (int i = 0; i < n; i++)
	{
		edge_file << logVector[i].source << "\t" << logVector[i].target << "\t" << logVector[i].recScore
				  << "\t" << logVector[i].fairScore << "\n";
	}
	edge_file.close();
}

void pagerank_algorithms::saveVector(std::string fileName, std::vector<step_log> &logVector)
{
	// Declare local variables.
	int n = logVector.size();

	// Open log file.
	std::ofstream logFile(fileName);

	// Write logs to file.
	logFile << "Edge\tPagerank\tPrediction\tSum_Prediction\n";
	for (int i = 0; i < n; i++)
	{
		logFile << i << "\t" << logVector[i].red_pagerank << "\t" << logVector[i].red_pagerank_prediction << "\t" << logVector[i].red_pagerank_generalized_prediction << std::endl;
	}

	// Close file.
	logFile.close();
}

void pagerank_algorithms::saveVector(std::string fileName, std::vector<recEdge> &logVector)
{
	std::ofstream edge_file(fileName);
	edge_file << "Source\tTarget\tnode2vecScore\tresAllocScore\tjaccCoefScore\tprefAttScore\tadamicAdarScore\tgain\texpGain\n";
	int n = logVector.size();
	for (int i = 0; i < n; i++)
	{
		edge_file << logVector[i].source << "\t" << logVector[i].target << "\t" << logVector[i].node2vecScore
				  << "\t" << logVector[i].resAllocScore << "\t" << logVector[i].jaccCoefScore << "\t" << logVector[i].prefAttScore
				  << "\t" << logVector[i].adamicAdarScore << "\t" << logVector[i].gain << "\t" << logVector[i].expGain << "\n";
	}
	edge_file.close();
}

// Reads random source nodes.
std::vector<int> pagerank_algorithms::getRandomSourceNodes()
{
	std::vector<int> sourceNodes;
	std::string str;
	int node;

	std::ifstream pagerankNodes("randomSourceNodes.txt");

	getline(pagerankNodes, str);
	while (getline(pagerankNodes, str))
	{
		node = std::stoi(str);
		sourceNodes.push_back(node);
	}
	pagerankNodes.close();

	return sourceNodes;
}

// Reads best by pagerank nodes.
std::vector<int> pagerank_algorithms::getBestByPagerankNodes()
{
	std::vector<int> sourceNodes;
	std::string str;
	int node;

	std::ifstream pagerankNodes("pagerankBestSourceNodes.txt");

	getline(pagerankNodes, str);
	while (getline(pagerankNodes, str))
	{
		node = std::stoi(str);
		sourceNodes.push_back(node);
	}
	pagerankNodes.close();

	return sourceNodes;
}

// Reads best by pagerank nodes.
std::vector<int> pagerank_algorithms::getBestRedSourceNodes()
{
	std::vector<int> sourceNodes;
	std::string str;
	int node;

	std::ifstream pagerankNodes("redBestSourceNodes.txt");

	getline(pagerankNodes, str);
	while (getline(pagerankNodes, str))
	{
		node = std::stoi(str);
		sourceNodes.push_back(node);
	}
	pagerankNodes.close();

	return sourceNodes;
}

// Reads best by pagerank nodes.
std::vector<int> pagerank_algorithms::getBestBlueSourceNodes()
{
	std::vector<int> sourceNodes;
	std::string str;
	int node;

	std::ifstream pagerankNodes("blueBestSourceNodes.txt");

	getline(pagerankNodes, str);
	while (getline(pagerankNodes, str))
	{
		node = std::stoi(str);
		sourceNodes.push_back(node);
	}
	pagerankNodes.close();

	return sourceNodes;
}

//
void pagerank_algorithms::compute_personalization_vector(std::vector<double> &pagerankv, double total_pagerank)
{
	switch (personalization_type)
	{
		case personalization_t::NO_PERSONALIZATION:
			compute_no_personalization_vector(pagerankv, total_pagerank);
			break;
		case personalization_t::ATTRIBUTE_PERSONALIZATION:
			compute_attribute_personalization_vector(pagerankv, total_pagerank, personalization_extra_info);
			break;
		case personalization_t::NODE_PERSONALIZATION:
			compute_node_personalization_vector(pagerankv, total_pagerank, personalization_extra_info);
			break;
		case personalization_t::JUMP_OPT_PERSONALIZATION:
			compute_custom_personalization_vector(pagerankv, total_pagerank);
			break;
		default:
			std::cerr << "Invalid personalization option." << std::endl;
			break;
	}
}


void pagerank_algorithms::compute_no_personalization_vector(std::vector<double> &pagerankv, double total_pagerank)
{
	#pragma omp parallel for
	for (unsigned int node = 0; node < pagerankv.size(); ++node) {
		const int comm = g.get_community(node);
		pagerankv[node] = total_pagerank * g.get_community_percentage(comm) / g.get_community_size(comm);
	}
}

void pagerank_algorithms::compute_step_proportional_no_personalization_vector(std::vector<double> &pagerankv,
		double total_pagerank, std::vector<node_info_t> &node_info)
{
	#pragma omp parallel for
	for (unsigned int node = 0; node < pagerankv.size(); ++node) {
		const int comm = g.get_community(node);
		pagerankv[node] = total_pagerank * g.get_community_percentage(comm) * node_info[node].importance_in_community;;
	}
}

void pagerank_algorithms::compute_attribute_personalization_vector(std::vector<double> &pagerankv,
		double total_pagerank, int attribute_id)
{
	auto nodes = g.get_nodes_with_attribute(attribute_id);
	for (const auto &node_id : nodes) {
		pagerankv[node_id] = total_pagerank / (double)nodes.size();
	}
}

void pagerank_algorithms::compute_node_personalization_vector(std::vector<double> &pagerankv,
		double total_pagerank, int node_id)
{
	if ((node_id >= 0) && ((unsigned long)node_id < pagerankv.size()))
		pagerankv[node_id] = total_pagerank;
}

void pagerank_algorithms::compute_custom_personalization_vector(std::vector<double> &pagerankv,
		double total_pagerank)
{
	int nnodes = g.get_num_nodes();
	for (int i = 0; i <nnodes; i++) {
		pagerankv[i] = total_pagerank * jump_vector[i];
	}
}