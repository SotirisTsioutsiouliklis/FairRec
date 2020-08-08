""" Script to calculate the candidate edges for the random source nodes

It selects all edges that are in distance k where distance k is choosen
such that candidate neighbors are at least as many as the MAX_EDGES and
the most as many as #neighbors in distance k - neighbors in distance 1
. It also selects all edges that have the highest personalized red
pagerank. It selects at least as many as the MAX_EDGES and the most as
many as MAX_EDGES + neighbors.

Creates:
--------

<nodeID>CandidateEdges.txt (txt file): list of condidates edges for
        each random source node.

TODO: Add best red and blue by criterion source nodes.
"""
import numpy as np
import networkx as nx
import subprocess
import sys
import time

MAX_EDGES = 40000000

start = time.time()

print('Init objects')
# Init graph.
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.DiGraph() )
# Read random source nodes.
randomSources = np.loadtxt('randomSourceNodes.txt', skiprows=1, dtype= int)

# Run cpp script for red personalized pageranks.
print("Get Red absorbing Probs...")
try:
    redRatios = np.loadtxt('out_personilized_red.txt', skiprows=1, usecols=1)
except:
    try: # Copy executable for real dataset.
        status = subprocess.call(['cp ../../../FairRec/Code/CppFiles/absorbingProbs.out .'], cwd=".", shell=True)
        if status != 0:
            raise Exception
        status = subprocess.call(['./absorbingProbs.out -r'], cwd=".", shell=True) # Read red pageranks.
        if status != 0:
            raise Exception
        redRatios = np.loadtxt('out_personilized_red.txt', skiprows=1, usecols=1)
    except:
        try: # Copy executable for synthetics.
            status = subprocess.call(['cp ../../../../../FairRec/Code/CppFiles/absorbingProbs.out .'], cwd=".", shell=True)
            if status != 0:
                raise Exception
            status = subprocess.call(['./absorbingProbs.out -r'], cwd=".", shell=True) # Read red pageranks.
            if status != 0:
                raise Exception
            redRatios = np.loadtxt('out_personilized_red.txt', skiprows=1, usecols=1)
        except:
            sys.exit('Unable to get red absorbing probs.')

# For each random source node.
order = 0
for node in randomSources:
    # Find #edges per source.
    maxNei = graph.number_of_nodes() - graph.out_degree(node) - 1
    edgesPerSource = min(MAX_EDGES // randomSources.size, maxNei)

    order += 1
    print('\tnode %d : %d' %(order, node))
    # Get Current Neighbors.
    neighbors = set()
    candidates = set()

    for nei in graph.neighbors(node):
        neighbors.add(nei)

    neighbors.add(node)

    print("\tFind best by red personalized pagerank")
    # Find best by red absorbing prob.
    if edgesPerSource < maxNei :
        tempRed = redRatios.copy()
        for i in range(edgesPerSource):
            best = 0
            bestVal = 0.
            for j in range(len(tempRed) ):
                if tempRed[j] > bestVal:
                    bestVal = tempRed[j]
                    best = j
            candidates.add(best)
            # Remove best from next iteration.
            tempRed[best] = -1.
    else:
        for i in range(graph.number_of_nodes() ):
            if i not in neighbors:
                candidates.add(i)

    # Log in file.
    with open('%dCandidateEdges.txt' %node, 'w') as fileOne:
        for target in candidates:
            fileOne.write('%d\t%d\n' %(node, target) )

end = time.time()
elapseTime = end - start
print("Total time of selecting candidate edges is: %f sec" %elapseTime)