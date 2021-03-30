""" Calculates the networks quality features.
Quality features: #nodes, #edges, homophily,
degree distribution (in -out), red ratio, red pagerank. 
"""
import numpy as np
import pandas as pd
import networkx as nx
import time


def getNodeCommunities():
    """Returns dictionary with node communities.
    Read out_communtiy.txt and creates a dictionary with key the nodes
    ids and arguments the nodes community.
    """
    nodeCommunities = dict()
    with open("out_community.txt", "r") as fileOne:
        fileOne.readline()
        for line in fileOne:
            node = int(line.split()[0])
            community = int(line.split()[1])
            nodeCommunities[node] = {"community": community}

    return nodeCommunities


def getRedRatio(graph):
    """Return the ratio of red nodes in graph."""
    numberOfRedNodes = 0
    numberOfNodes = graph.number_of_nodes()
    communities = nx.get_node_attributes(graph, "community")

    for i in range(numberOfNodes):
        if communities[i] == 1:
            numberOfRedNodes += 1

    redRatio = numberOfRedNodes / numberOfNodes

    return redRatio


def getHomophily(graph):
    """Return homophily of the graph.
    Homophily = cross edges ratio / expected cross edges ratio.
    homophily = 1 means no homophily. homophily > 1 means heterohily.
    homphily < 1 means homophily.
    """
    global redRatio
    blueRatio = 1 - redRatio
    crossEdges = 0

    communities = nx.get_node_attributes(graph, "community")

    for edge in graph.edges:
        if communities[edge[0]] != communities[edge[1]]:
            crossEdges += 1

    crossEdgesRatio = crossEdges / graph.number_of_edges()

    expectedCrossEdges = 2 * redRatio * blueRatio
    homophily = crossEdgesRatio / expectedCrossEdges

    return homophily


def getInDegreeDistribution(graph):
    inDegrees = list(graph.in_degree())
    inDegreesArray = np.array([inDegree for node, inDegree in inDegrees])
    inDegreesDistribution = np.histogram(inDegreesArray, bins=max(inDegreesArray) + 1)

    return inDegreesDistribution[0]


def getOutDegreeDistribution(graph):
    outDegrees = list(graph.out_degree())
    outDegreesArray = np.array([outDegree for node, outDegree in outDegrees])
    outDegreesDistribution = np.histogram(
        outDegreesArray, bins=max(outDegreesArray) + 1
    )

    return outDegreesDistribution[0]


def getRedPagerank():
    """Calculates the redPagerank of the network."""
    # pagerank = np.loadtxt("out_pagerank.txt", skiprows=1)[:, 1]
    pagerank = pd.read_csv("out_pagerank.csv")["Pagerank"].to_numpy()
    communities = np.loadtxt("out_community.txt", skiprows=1, dtype=int)

    redPagerank = 0.0
    for i in range(pagerank.size):
        if communities[i][1] == 1:
            redPagerank += pagerank[communities[i][0]]

    return redPagerank


# ------------------------------------------------------------------------------------
# Start timing.
startTime = time.time()

# Load graph.
graph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.DiGraph())

# Read node attributes.
nodeAttributes = getNodeCommunities()
nx.set_node_attributes(graph, nodeAttributes)

# Get red ratio.
redRatio = getRedRatio(graph)

# Store results.
with open("networksQualityFeatures.txt", "w") as fileOne:
    fileOne.write("NumberOfNodes\t%d\n" % graph.number_of_nodes())
    fileOne.write("NumberOfEdges\t%d\n" % graph.number_of_edges())
    fileOne.write("RedRatio\t%f\n" % redRatio)
    fileOne.write("RedPagerank\t%f\n" % getRedPagerank())
    fileOne.write("Homophily\t%f\n" % getHomophily(graph))

# Get in degree distribution.
inDegreeDistribution = getInDegreeDistribution(graph)
outDegreeDistribution = getOutDegreeDistribution(graph)

# Store distributions.
np.savetxt("inDegreeDistribution.txt", inDegreeDistribution, delimiter="\t")
np.savetxt("outDegreeDistribution.txt", outDegreeDistribution, delimiter="\t")

# Stop timing
stopTime = time.time()

# Log time.
elapsedTime = stopTime - startTime
with open("networksQualityFeatures_timing.txt", "w") as fileOne:
    fileOne.write("Elapsed time:\t%f seconds" % elapsedTime)
