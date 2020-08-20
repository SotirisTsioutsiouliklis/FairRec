""" Calculates distances for the selected edges.
"""
import numpy as np
import networkx as nx
import time
import sys

def getCandidateEdgesDistances(kind):
    """
    """
    edgesDistances = dict()

    with open("edgesDistances" + kind + "Sources.txt", "r") as fileOne:
        fileOne.readline()
        for line in fileOne:
            info = line.split()
            key = int(info[0])
            val = int(info[1])
            dis = int(info[2])
            #path = int(info[3])
            if key not in edgesDistances:
                edgesDistances[key] = dict()
            #edgesDistances[key][val] = (dis, path)
            edgesDistances[key][val] = dis

    return edgesDistances

def writeSelectedEdgesDistances(edgesDistances, policy, kind):
    """
    """
    # Get selected edges by policy and kind.
    selectedEdges = set()
    with open("edgesSelectedBy%s%sSources.txt" %(policy, kind), "r") as fileOne:
        fileOne.readline()
        for line in fileOne:
            edge = (int(line.split()[0]), int(line.split()[1]))
            selectedEdges.add(edge)

    # Write selected edges by policy and kind.
    with open("selectedEdgesBy%s%sSourcesDistances.txt" %(policy, kind), "w") as fileOne:
        fileOne.write("source\ttarget\tdistance\n")
        for sourceNode in edgesDistances:
            for targetNode in edgesDistances[sourceNode]:
                if (sourceNode, targetNode) in selectedEdges:
                    fileOne.write("%d\t%d\t%d\n" %(sourceNode, targetNode,
                                edgesDistances[sourceNode][targetNode])

# ---------------------------------------------------------------
# Start timing.
startTime = time.time()


# Different policies.
policies = ['Node2vec', 'ResourceAllocation', 'PreferencialAttachment', 'JaccardCoefficient', 'Gain', 'ExpGain', 
        'AdamicAdar', 'Random']

kinds = ['Random', 'Red', 'Blue']

# Get candidate edges distances for random nodes.
edgesDistances = getCandidateEdgesDistances("Random")

# Get selected edges distances.
for kind in kinds:
    for policy in policies:
        writeSelectedEdgesDistances(edgesDistances, policy, kind)

# Stop timing
stopTime = time.time()

# Log time.
elapsedTime = stopTime - startTime
with open("selectedEdgesDistances-timing.txt", "w") as fileOne:
    fileOne.write("Total time:\t%f seconds" %elapsedTime)