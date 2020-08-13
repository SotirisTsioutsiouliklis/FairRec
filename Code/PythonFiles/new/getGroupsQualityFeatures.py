""" Calculates group(red, blue) quality features.

Quality features: homophily, ratio in network.
"""
import numpy as np
import networkx as nx
import time

def getNodeCommunities():
    """ Returns dictionary with node communities.

    Read out_communtiy.txt and creates a dictionary with key the nodes
    ids and arguments the nodes community.
    """
    nodeCommunities = dict()
    with open("out_communities.txt", "r") as fileOne:
        fileOne.readline()
        for line in fileOne:
            node = int(line.split()[0])
            community = int(line.split()[1])
            nodeCommunities[node] = {"community" : community}

    return nodeCommunities

def getRedRatio(graph):
    """ Return the ratio of red nodes in graph.
    """
    numberOfRedNodes = 0
    numberOfNodes = graph.number_of_nodes()

    for i in range(numberOfNodes):
        if graph[i]["community"] == 1:
            numberOfRedNodes += 1

    getRedRatio = numberOfRedNodes / numberOfNodes

    return redRatio

def getGroupHomphily(graph):
    """ Return the homophily of each group in tuple.

    Homophily of group 0 is the ratio of cross edges from group 0 to
    group 1 / expected ratio of cross edges from group 0 to group 1.
    homphily of group 1 is defined analogously.

    Return
    -------
    homophilies (2D tuple of floats): homopilies[0] is the homophily
        of group 0 and homophily[1] the homophily of group 1.
    """
    global redRatio
    blueRatio = 1 - redRatio
    
    crossEdgesFromBlue = 0
    crossEdgesFromRed = 0
    allEdgesFromBlue = 0
    allEdgesFromRed = 0

    for edge in graph.edges:
        if edge[0]["community"] == 0:
            allEdgesFromBlue += 1
            if edge[1]["community"] == 1:
                crossEdgesFromBlue += 1
        if edge[0]["community"] == 1:
            allEdgesFromRed += 1
            if edge[1]["community"] == 0:
                crossEdgesFromRed += 1

    crossEdgesRatioFromRed = crossEdgesFromRed / allEdgesFromRed
    crossEdgesRatioFromBlue = crossEdgesFromBlue / allEdgesFromBlue
    expectedCrossEdgesFromRed = blueRatio
    expectedCrossEdgesFromBlue = redRatio 

    groupHomophilies = (crossEdgesRatioFromBlue / expectedCrossEdgesFromBlue, crossEdgesRatioFromRed / expectedCrossEdgesFromRed)

    return groupHomophilies

# -------------------------------------------------
# Start timing.
startTime = time.time()

# Load graph.
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.DiGraph() )

# Get red ratio.
redRatio = getRedRatio(graph)

# Read node attributes.
nodeAttributes = getNodeCommunities()
nx.set_node_attributes(graph, nodeAttributes)

# Getgroup homophily.
groupHomophily = getGroupHomphily(graph)

# Store results.
with open("groupQualityFeatures.txt", "W") as fileOne:
    fileOne.write("Group\tRatio\thomophily\n")
    fileOne.write("%d\t%f\t%f\n" %(0, 1 - redRatio, groupHomophily[0]) )
    fileOne.write("%d\t%f\t%f\n" %(1, redRatio, groupHomophily[1]) )

# Stop timing.
stopTime = time.time()

# Log time.
elapsedTime = stopTime - startTime
with open("groupQualityFeatures-timing.txt", "w") as fileOne:
    fileOne.write("Total time:\t%f seconds" %elapsedTime)