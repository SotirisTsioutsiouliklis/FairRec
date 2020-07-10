/**
 * In this experiment we compare the effect of adding edges by
 * recommendation score, by gain score or by expected gain score(
 * our poor approximation of it).
 * 
 * To do that we choose 20% random nodes from the network. We then
 * calculate the forthmentioned score for each candidate edge of each
 * source node. For every criterion we add the first edge for all
 * source nodes and we log the red ratio of the network. Then the
 * second for all source node and we log again the red ratio of the
 * network, we continue until ww add 10 edges to all selected source
 * nodes.
 * 
 * @NOTE: The source nodes that we use have no relation to the typical
 * source nodes we use for the most of others experiments.
 * 
 * Target of the experiment:
 *      1. See the total impact of recommenders to the red ratio of a
 *      network.
 * 
 * Out files:
 *      1. "impactByRec.txt": Pending description...
 *      2. "impactByGain.txt": Pending description...
 *      3. "impactByExpGain.txt": Pending description...
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <set>
#include <cmath>
#include "graph.hpp"
#include "pagerank.hpp"
#include <chrono>
#include <omp.h>
#include <string>
#include <sstream>