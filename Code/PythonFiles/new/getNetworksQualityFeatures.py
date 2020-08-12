""" Calculates the networks quality features.

Quality features: #nodes, #edges, homophily,
degree distribution (in -out), red ratio, red pagerank. 
"""
import numpy as np
import networkx as nx

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

def getHomophily(graph):
    """ Return homophily of the graph.

    Homophily = cross edges / expected cross edges.
    homophily = 1 means no homophily. homophily > 1 means heterohily.
    homphily < 1 means homophily.
    """
    global redRatio
    blueRatio = 1 - redRatio
    crossEdges = 0

    for edge in graph.edges:
        if edge[0]["community"] != edge[1]["community"]:
            crossEdges += 1
    
    expectedCrossEdges = 2 * redRatio * blueRatio
    homophily = crossEdges / expectedCrossEdges

    return homophily

def getInDegreeDistribution(graph):
    inDegrees = list(graph.in_degree() )
    inDegreesArray = np.array([inDegree for node, inDegree in inDegrees])
    inDegreesDistribution = np.histogram(inDegreesArray, bins= max(inDegreesArray) + 1)

    return inDegreesDistribution

def getOutDegreeDistribution(graph):
    outDegrees = list(graph.out_degree() )
    outDegreesArray = np.array([outDegree for node, outDegree in outDegrees])
    outDegreesDistribution = np.histogram(outDegreesArray, bins= max(outDegreesArray) + 1)

    return outDegreesDistribution

def getRedPagerank():
    """ Calculates the redPagerank of the network.
    """
    pagerank = np.loadtxt("out_pagerank_pagerank.txt")
    communities = np.loadtxt("out_communities.txt", skiprows = 1)[:,1]

    redPagerank = 0
    for i in range(pagerank.size):
        if communities[i] == 1:
            redPagerank += pagerank[i]
    
    return redPagerank

# ------------------------------------------------------------------------------------
# Load graph.
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.DiGraph() )

# Read node attributes.
nodeAttributes = getNodeCommunities()
nx.set_node_attributes(graph, nodeAttributes)

# Get red ratio.
redRatio = getRedRatio(graph)

# Store results.
with open("networksQualityFeatures.txt", "w") as fileOne:
    fileOne.write("NumberOfNodes\t%d\n" %graph.number_of_nodes() )
    fileOne.write("NumberOfEdges\t%d\n" %graph.number_of_edges() )
    fileOne.write("RedRatio\t%d\n" %redRatio )
    fileOne.write("RedPagerank\t%f\n" %getRedPagerank() )
    fileOne.write("Homophily\t%f\n" %getHomophily(graph) )

# Get in degree distribution.
inDegreeDistribution = getInDegreeDistribution(graph)
outDegreeDistribution = getOutDegreeDistribution(graph)

# Store distributions.
np.savetxt("inDegreeDistribution.txt", inDegreeDistribution, delimiter= '\t')
np.savetxt("outDegreeDistribution.txt", outDegreeDistribution, delimiter= '\t')