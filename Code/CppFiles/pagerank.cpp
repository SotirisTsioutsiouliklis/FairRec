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

using namespace std;

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
	for (i = 0; i < nnodes; ++i)
	{
		pagerankv[i].node_id = i;
		pagerankv[i].pagerank = 1.0 / nnodes;
	}

	// compute pagerank for each node
	std::vector<double> tmp_pagerank(nnodes);
	std::vector<double> tmp_pagerank_jump(nnodes); // for personalization
	int iter = 0;
	int neigh_degree;
	for (; iter < max_iter; ++iter)
	{
		double sum = 0.0;
#pragma omp parallel for firstprivate(nnodes) reduction(+ \
														: sum)
		for (node = 0; node < nnodes; ++node)
		{
			tmp_pagerank[node] = 0.0;
			for (const int &neighbor : g.get_in_neighbors(node))
			{
				neigh_degree = g.get_out_degree(neighbor);
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
#pragma omp parallel for firstprivate(nnodes, tmp_pagerank, tmp_pagerank_jump) private(new_val) reduction(+ \
																										  : diff)
		for (node = 0; node < nnodes; ++node)
		{
			new_val = tmp_pagerank[node] + leaked + tmp_pagerank_jump[node];
			diff += std::fabs(new_val - pagerankv[node].pagerank);
			pagerankv[node].pagerank = new_val;
		}
		if (diff < eps)
			break;
	}

	if (iter == max_iter)
		std::cerr << "[WARN]: Pagerank algorithm reached " << max_iter << " iterations." << std::endl;
	//cached_pagerank = pagerankv;
	//is_cache_valid = true;
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
	rankVector = get_pagerank();
	// Get red pagerank.
	redPagerank = g.get_pagerank_per_community(rankVector)[1];

	// Run absoring to Red.
	redAbsorbingProbs = get_red_abs_prob();
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
		objectiveValues[targetNode].pagerank = redPagerank + rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
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
			// P(R)' = ....
			//tempNode.pagerank = redPagerank + rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
			tempNode.pagerank = redPagerank - rankVector[sourceNode].pagerank * (objectiveNominator / objectiveDenominator);
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