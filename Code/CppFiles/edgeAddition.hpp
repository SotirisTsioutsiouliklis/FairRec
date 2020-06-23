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
    int sourceOutDegree;
    double recScore, fairScore;

    bool operator == (const edge &other) {
        return (source == other.source && target == other.target);
    }
};

class EdgeAddition {
    public:
        EdgeAddition(graph &g, pagerank_algorithms &algs);

        /**
         * A greedy algorithm to add edges to a graph in
         * order to maximize red pagerank ratio. It adds k edges to a single
         * source of the graph based on formula of the FairRec paper.
         * 
         * Creates two files:
         *      1. "<sourceNode>RedPagerankGreedy.txt": Red ratio of pagerank
         *          per edge added.
         *      2. "<sourceNode>edgesGreedy.txt": Edges that added to graph.
         * 
         * @param sourceNode (int): The source node for the edges.
         * @param numberOfEdges (int): The number of edges to add.
        */
        static void getGreedySingleSource(int sourceNode, int numberOfEdges);
        /**
         * The "fast greedy" algorithm described in fairRec paper to add edges
         * to a graph in order to maximize red pagerank ratio. It adds k edges
         * to a single source of the graph based on formula of the FairRec
         * paper.
         *
         * Creates two files:
         *      1. "<sourceNode>RedPagerankGreedy.txt": Red ratio of pagerank per edge
         *          added.
         *      2. "<sourceNode>edgesGreedy.txt": Edges that added to graph.
         * 
         * @param sourceNode (int): The source node for the edges.
         * @param numberOfEdges (int): The number of edges to add.
        */
        static void getFastGreedySingleSource(int sourceNode, int numberOfEdges);
        /**
         * A greedy algorithm to add edges to a graph in
         * order to maximize red pagerank ratio. It adds <numberOfEdges>
         * edges to <numberOfSources> nodes. Source nodes are selected
         * best by pagerank. Edges are selected based on formula of the
         * FairRec paper.
         * 
         * Creates two files:
         *      1. "RedPagerankGreedy.txt": Red ratio of pagerank
         *          per edge added.
         *      2. "edgesGreedy.txt": Edges that added to graph.
         * 
         * @param sourceNode (int): The source node for the edges.
         * @param numberOfEdges (int): The number of edges to add.
        */
        static void getGreedyMultySource(int numberOfSources, int numberOfEdges);
        /**
         * the "fast greedy" algorithm described in fairRec paper to
         * add edges to a graph in order to maximize red pagerank
         * ratio. It adds <numberOfEdges> edges to <numberOfSources>
         * nodes. Source nodes are selected best by pagerank. Edges are
         * selected based on formula of the FairRec paper.
         * 
         * Creates two files:
         *      1. "RedPagerankGreedy.txt": Red ratio of pagerank
         *          per edge added.
         *      2. "edgesGreedy.txt": Edges that added to graph.
         * 
         * @param sourceNode (int): The source node for the edges.
         * @param numberOfEdges (int): The number of edges to add.
        */
        static void getFastGreedyMultySource(int numberOfSources, int numberOfEdges);

        /**
         * By objective value we mean the prediction for the Red pagerank ratio
         * in case we add an edge in the graph. The prediction is based on the
         * formula of the FairRec paper and it is accurate.
        */
        pagerank_v getObjectiveValues(int sourceNode);

        // Returns n random sources.
        static std::vector<int> getRandomSourceNodes(int numberOfSourceNodes, int numberOfGraphNodes);
        // Save various vectors.
        static void saveVector(std::string fileName, pagerank_v &logVector);
        static void saveVector(std::string fileName, std::vector<int> &logVector);
        static void saveVector(std::string fileName, std::vector<double> &logVector);
        static void saveVector(std::string fileName, std::vector<edge> &logVector);
        static void saveVector(std::string fileName, std::vector<step_log> &logVector);
    private:
        // Attributes
        graph &g;
        pagerank_algorithms &algs;

        // Methods
        
        
        // Returns the best node to connect at the moment.
        int getBestTargetNode(int sourceNode);
        // Returns the best k nodes to connect at the moment.
        std::vector<int> getBestTargetNodes(int sourceNode, int k);


        
};

#endif /* _EDGEADDITION_HPP */