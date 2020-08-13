""" Calculates nodes quality features.

Quality features: pagerank, red pagerank, group, degree (in-out),
red neighbors ratio.
"""
import numpy as np
import networkx as nx
import time

def getRedPagerank(pagerank, communities):
    """ Calculates the redPagerank of the network.
    """
    redPagerank = 0
    for i in range(pagerank.size):
        if communities[i] == 1:
            redPagerank += pagerank[i]
    
    return redPagerank

def getRedInNeighbors(graph, node):
    redInNeighbors = np.zeros(graph.number_of_nodes(), dtype = int)

    for i in range(graph.number_of_nodes() ):
        inNeighbors = [nei for nei in graph.predecessors(i) if communities[nei] == 1]
        redInNeighbors[i] = len(inNeighbors)
    
    return redInNeighbors

def getRedOutNeighbors(graph, node):
    redOutNeighbors = np.zeros(graph.number_of_nodes(), dtype = int)
    for i in range(graph.number_of_nodes() ):
        outNeighbors = [nei for nei in graph.neighbors(i) if communities[nei] == 1] # .neighbors equal to .successors for digraphs.
        redOutNeighbors[i] = len(outNeighbors)
    
    return redOutNeighbors

def getInRedNeighborRatio(graph, communities):
    redInNeighbors = getRedInNeighbors(graph, communities)
    redInNeighborhoodRatio = [redInNeighbors[i] / graph.in_degree(i) for i in range(graph.number_of_nodes() )] 

    return redInNeighborhoodRatio

def getOutRedNeighborRatio(graph, communities):
    redOutNeighbors = getRedOutNeighbors(graph, communities)
    redOutNeighborhoodRatio = [redOutNeighbors[i] / graph.out_degree(i) for i in range(graph.number_of_nodes() )] 

    return redOutNeighborhoodRatio

# ---------------------------------------------------------------
# Start timing.
startTime = time.time()

# Load graph.
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.DiGraph() )
# Get pagerank.
pagerank = np.loadtxt("out_pagerank.txt", skiprows= 1)[:,1]
# Get communities. TODO: improve.
preCommunities = np.loadtxt("out_community.txt", skiprows = 1, dtype= int)
communities = np.zeros(graph.number_of_nodes() )
for i in range(graph.number_of_nodes() ):
    communities[preCommunities[i][0]] = preCommunities[i][1]
# Get red pagerank.
redPagerank = getRedPagerank(pagerank, communities)
# Get red neighborhood ratio.
redInNeighborsRatio = getInRedNeighborRatio(graph, communities)
redOutNeighborsRatio = getOutRedNeighborRatio(graph, communities)

# Store results.
with open("nodeQualityFeatures.txt", "w") as fileOne:
    fileOne.write("nodeId\tpagerank\tredPagerank\tgroup\tinDegree\toutDegree\tredNeighborsInRatio\tredNeighborsOutRatio\n")
    for i in range(graph.number_of_nodes() ):
        fileOne.write("%d\t%f\t%f\t%d\t%d\t%d\t%f\t%f\n" %(i, pagerank[i], redPagerank[i], communities[i], 
                    graph.in_degree(i), graph.out_degree(i), redInNeighborsRatio[i], redOutNeighborsRatio[i] ) )

# Stop timing.
stopTime = time.time()

# Log time.
elapsedTime = stopTime - startTime
with open("nodeQualityFeatures-timing.txt", "w") as fileOne:
    fileOne.write("Total time:\t%f seconds" %elapsedTime)