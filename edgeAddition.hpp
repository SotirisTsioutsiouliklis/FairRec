/**
 * This file contains help methods for the implementation of 
 * edge addition algorithms. 
*/ 
#ifndef _EDGEADDITION_HPP
#define _EDGEADDITION_HPP

#include "graph.hpp"
#include "pagerank.hpp"

struct step_log {
    double red_pagerank;
    double red_pagerank_prediction;
    double red_pagerank_generalized_prediction;
};

struct edge {
    int source;
    int target;
    int s_outd;
    double s_score_1, s_score_2, t_score, e_score;

    bool operator == (const edge &other) {
        return (source == other.source && target == other.target);
    }
};

class EdgeAddition {
    public:
        EdgeAddition(graph &g, pagerank_algorithms &algs);
        // Returns n random sources.
        std::vector<int> getRandomSourceNodes(int n);
        // Returns the best node to connect at the moment.
        int getBestTargetNode(int sourceNode);

        // Save various vectors.
        static void saveVector(std::string fileName, pagerank_v &logVector);
        static void saveVector(std::string fileName, std::vector<int> &logVector);
        static void saveVector(std::string fileName, std::vector<double> &logVector);
        static void saveVector(std::string fileName, std::vector<edge> &logVector);
        static void saveVector(std::string fileName, std::vector<step_log> &logVector);

    private:
        // Methods
        // Return values of Objective function.
        pagerank_v getObjectiveValues(int sourceNodes);

        // Attributes
        graph &g;
        pagerank_algorithms &algs;
};

#endif /* _EDGEADDITION_HPP */