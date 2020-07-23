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
# Find #edges per source.
edgesPerSource = min(MAX_EDGES // randomSources.size, graph.number_of_nodes() // 3)

# Run cpp script for red personalized pageranks.
print("Get Red absorbing Probs...")
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

print('Find candidates for node: ')
# For each random source node.
order = 0
for node in randomSources:
    order += 1
    print('\tnode %d : %d' %(order, node))
    # Get Current Neighbors.
    explored = [False for i in range(graph.number_of_nodes() )]
    candidates = [False for i in range(graph.number_of_nodes() )]
    neighbors = [False for i in range(graph.number_of_nodes() )]

    toExplore = set([node])
    temp = set()
    for nei in graph.neighbors(node):
        neighbors[nei] = True

    neighbors[node] = True

    print("\tFind best by distance")
    # Find proper distance for rec candidates.
    numberOfOutNeighbors = graph.out_degree(node)
    numberOfCandidates = sum(candidates)

    while numberOfCandidates < edgesPerSource:
        for n in toExplore:
            if not explored[n]:
                for nei in nx.all_neighbors(graph, n):
                    if not neighbors[nei]:
                        candidates[nei] = True
                    if not explored[nei]:
                        temp.add(nei)
        
            explored[n] = True

        toExplore = temp.copy()
        temp.clear()
        numberOfCandidates = sum(candidates)

    candidateNeighbors = set([i for i in range(len(candidates) ) if candidates[i] ])

    print("\tFind best by red personalized pagerank")
    # Find best edges based on red pagerank.
    # TODO: Improve to find them in the beggining.

    print("\tConcatenate, clean and log")
    # Get best by red pagerank.
    bestByRedPagerank = np.argsort(-redRatios)
    # Keep only as many as you need.
    bestByRedPagerank = bestByRedPagerank[0:edgesPerSource + numberOfOutNeighbors]

    # get candidate nodes.
    candidateNeighbors.union(set(bestByRedPagerank))
    candidateNeighbors.difference(neighbors)

    # Log in file.
    with open('%dCandidateEdges.txt' %node, 'w') as fileOne:
        for target in candidateNeighbors:
            fileOne.write('%d\t%d\n' %(node, target) )

end = time.time()
elapseTime = end - start
print("Total time of selecting candidate edges is: %f sec" %elapseTime)