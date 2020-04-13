#ifndef _EDGE_ADD_HPP
#define _EDGE_ADD_HPP

#include "graph.hpp"
#include "pagerank.hpp"

enum class algorithm_mode {GREEDY, FAST_GREEDY, APPROX, FAST_APPROX};

class Edge_addition {
    public:
        Edge_addition(graph &g, pagerank_algorithms &algs, const int n_source);
        void greedy();
        void fast_greedy();
        //void approx();
        //void fast_approx();
    
    private:
        graph &g;
        pagerank_algorithms &algs;
        int n_source;
};
