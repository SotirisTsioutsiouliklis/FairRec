""" Calculates group(red, blue) quality features.

Quality features: homophily, ratio in network.
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
    otherGroup = 1 - group # group cn be 0 or 1.
    pass

# -------------------------------------------------
# Load graph.
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.DiGraph() )

# Read node attributes.
nodeAttributes = getNodeCommunities()
nx.set_node_attributes(graph, nodeAttributes)

