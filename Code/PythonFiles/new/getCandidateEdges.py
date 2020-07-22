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
# Find edges per source.
edgesPerSource = MAX_EDGES // randomSources.size

print('Find candidates for node: ')
# For each random source node.
order = 0
for node in randomSources:
    order += 1
    print('\tnode %d : %d' %(order, node))
    # Get Current Neighbors.
    tempGraph = nx.ego_graph(graph, node, 1) # Returns out ego graph.
    neighbors = set(tempGraph.nodes)
    print("\tFind best by distance")
    # Find proper distance for rec candidates.
    numberOfOutNeighbors = tempGraph.number_of_nodes()
    distance = 1
    numberOfCandidates = 0
    newCandidates = set()
    newCandidates.add(node)

    while numberOfCandidates < edgesPerSource + numberOfOutNeighbors + 1:
        for n in tempCandidates:
            for nei in graph.neighbors(n):
                tempNodes.add

        tempGraph = nx.ego_graph(graph, node, distance)
        numberOfCandidates = tempGraph.number_of_nodes() - (numberOfOutNeighbors + 1)
    
    # Get ego graph of proper distance.
    tempGraph = nx.ego_graph(graph, node, distance)
    candidateNeighbors = set(tempGraph.nodes)

    print("\tFind best by red personalized pagerank")
    # Find best edges based on red pagerank.
    # Run cpp script.
    try:
        subprocess.call(['./absorbingProbs.out -r'], cwd=".", shell=True)
    except: # Copy executable for real dataset.
        try:
            subprocess.call(['cp ../../../FairRec/Code/CppFiles/absorbingProbs.out .'], cwd=".", shell=True)
            subprocess.call(['./absorbingProbs.out -r'], cwd=".", shell=True)
        except:
            try: # Copy executable for synthetics.
                subprocess.call(['cp ../../../../../FairRec/Code/CppFiles/absorbingProbs.out .'], cwd=".", shell=True)
                subprocess.call(['./absorbingProbs.out -r'], cwd=".", shell=True)
            except:
                sys.exit('Unable to get red absorbing probs.')

    print("\tConcatenate, clean and log")
    # Read red pageranks.
    redRatios = np.loadtxt('out_personilized_red.txt', skiprows=1, usecols=1)
    # Get best by red pagerank.
    bestByRedPagerank = np.argsort(-redRatios)
    # Keep only as many as you need.
    bestByRedPagerank = bestByRedPagerank[0:edgesPerSource + numberOfOutNeighbors]

    # get candidate nodes.
    candidateNeighbors.union(set(bestByRedPagerank))
    candidateNeighbors.difference(neighbors)

    # Log in file.
    with open('%dCandidateEdges.txt', 'w') as fileOne:
        for source, target in candidateNeighbors:
            fileOne.write('%d\t%d\n' %(source, target) )

end = time.time()
elapseTime = end - start
pritn("Total time of selecting candidate edges is: %f sec" %elapseTime)