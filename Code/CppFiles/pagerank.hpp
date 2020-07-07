#ifndef _PAGERANK_HPP
#define _PAGERANK_HPP

#include <vector>
#include "graph.hpp"

struct step_log {
    double red_pagerank;
    double red_pagerank_prediction;
    double red_pagerank_generalized_prediction;
};

typedef struct {
	double node_percentage; // percentage of pagerank this node will GIVE to each of its neighbors
	std::vector<double> community_percentage; // percentage of pagerank this node wil GIVE to each community
	double importance_in_community; // percentage of community pagerank this node will GET
} node_info_t; // for step fair pagerank algorithms

enum class personalization_t {NO_PERSONALIZATION, ATTRIBUTE_PERSONALIZATION, NODE_PERSONALIZATION, JUMP_OPT_PERSONALIZATION};
enum class absorb_mode {RED, BLUE, NODE, NONE};

class pagerank_algorithms
{
public:
	pagerank_algorithms(graph &g);

	// Famous PageRank.
	pagerank_v get_pagerank(const double C=0.85, const double eps=1e-4, const int max_iter=100);
	// Personilized pagerank to Red for all nodes.
	pagerank_v get_red_abs_prob(const double C=0.85, const double eps=1e-4, const int max_iter=200);
	pagerank_v get_blue_abs_prob(const double C=0.85, const double eps=1e-4, const int max_iter=200);
	pagerank_v get_node_abs_prob(int abs_node, const double C=0.85, const double eps=1e-4, const int max_iter=200);

	/**
     * By objective value we mean the prediction for the Red pagerank ratio
     * in case we add an edge in the graph. The prediction is based on the
     * formula of the FairRec paper and it is accurate.
    */
    pagerank_v getObjectiveValues(int sourceNode);

	void sort_pagerank_vector(pagerank_v &pagerank);

	// Save various vectors.
    static void saveVector(std::string fileName, pagerank_v &logVector);
    static void saveVector(std::string fileName, std::vector<int> &logVector);
    static void saveVector(std::string fileName, std::vector<double> &logVector);
    static void saveVector(std::string fileName, std::vector<edge> &logVector);
    static void saveVector(std::string fileName, std::vector<step_log> &logVector);
private:
	void compute_pagerank_no_personalization_vector(std::vector<double> &pagerankv, double total_pagerank);

	graph &g;
	personalization_t personalization_type;
	int personalization_extra_info;
	std::vector<double> jump_vector;

	pagerank_v cached_pagerank; // to avoid recomputing it for the global fair algorithms
	bool is_cache_valid;
};

#endif /* _PAGERANK_HPP */
