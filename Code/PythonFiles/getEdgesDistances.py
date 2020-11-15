""" Calculates distances for the candidate edges.
"""
import numpy as np
import networkx as nx
import time
import sys

def getCandidateEdges(kind):
    """
    """
    edges = dict()

    with open("edgesScores" + kind + ".txt", "r") as fileOne:
        fileOne.readline()
        for line in fileOne:
            info = line.split()
            key = int(info[0])
            val = int(info[1])
            if key not in edges:
                edges[key] = set()
            edges[key].add(val)

    return edges

# ---------------------------------------------------------------
# Start timing.
startTime = time.time()

# Load graph.
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.Graph() )

# Get candidate edges for random nodes.
edges = getCandidateEdges("Random")

# Write the results.
with open("edgesDistancesRandomSources.txt", "w") as fileOne:
    fileOne.write("source\ttarget\tdistance\n")
    for sourceNode in edges:
        for targetNode in edges[sourceNode]:
            try:
                pathLength = nx.shortest_path_length(graph, source = sourceNode , target = targetNode)
            except:
                pathLength = 100
            #try:
             #   reversePathLength = nx.shortest_path_length(graph, source = targetNode , target = sourceNode)
            #except:
             #   reversePathLength = 100
            #distance = min(pathLength, reversePathLength)

            fileOne.write("%d\t%d\t%d\n" %(sourceNode, targetNode, pathLength) )

# Get candidate edges for random nodes.
edges = getCandidateEdges("Red")

# Write the results.
with open("edgesDistancesRedSources.txt", "w") as fileOne:
    fileOne.write("source\ttarget\tdistance\n")
    for sourceNode in edges:
        for targetNode in edges[sourceNode]:
            try:
                pathLength = nx.shortest_path_length(graph, source = sourceNode , target = targetNode)
            except:
                pathLength = 100
            #try:
             #   reversePathLength = nx.shortest_path_length(graph, source = targetNode , target = sourceNode)
            #except:
             #   reversePathLength = 100
            #distance = min(pathLength, reversePathLength)

            fileOne.write("%d\t%d\t%d\n" %(sourceNode, targetNode, pathLength) )

# Get candidate edges for random nodes.
edges = getCandidateEdges("Blue")   

# Write the results.
with open("edgesDistancesBlueSources.txt", "w") as fileOne:
    fileOne.write("source\ttarget\tdistance\n")
    for sourceNode in edges:
        for targetNode in edges[sourceNode]:
            try:
                pathLength = nx.shortest_path_length(graph, source = sourceNode , target = targetNode)
            except:
                pathLength = 100
            #try:
             #   reversePathLength = nx.shortest_path_length(graph, source = targetNode , target = sourceNode)
            #except:
             #   reversePathLength = 100
            #distance = min(pathLength, reversePathLength)

            fileOne.write("%d\t%d\t%d\n" %(sourceNode, targetNode, pathLength) )

# Stop timing.
stopTime = time.time()

# Log time.
elapsedTime = stopTime - startTime
with open("edgesDistances-timing.txt", "w") as fileOne:
    fileOne.write("Total time:\t%f seconds" %elapsedTime)