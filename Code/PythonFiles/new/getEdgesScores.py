""" Gets scores for candidate edges.

Gets the following scores for edges that doesn't exist in the graph and
that the target node is in maximum distance 3 from the source node. See
repository's README for exact definition of the scores below.

This script is not independable as it requires node2vec node
embeddings. We create a list of the files that it will need
to find inside the datasets folder and we raise the proper
messages / errors if the program isn't able to spot the on the folder
or copy them from the repository. In this case you should copy them
maunally.

Require files:
    1. 

Create file:
    1. edgeRecScores.txt: <sourceNode>\t<targetNode>\t<node2vecRecommendationScore>\t<resourceAllocationScore>\t
                        <jaccardCoefficientScore>\t<preferencialAttachmentScore>\t<addamicAddarScore>\tgain\texpectedGain\n

Run inside datasets folder.         
"""
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
import numpy as np
import subprocess
import sys
import pickle
import networkx as nx
import time
import fairRec as fr

# Load recommender.
linkRecommender = pickle.load(open('edgeClassifier.sav', 'rb') )

# Get Source Nodes.
randomNodes = np.loadtxt('randomSourceNodes.txt', skiprows= 1, dtype= int)
bestRedNodes = np.loadtxt('redBestSourceNodes.txt', skiprows= 1, dtype= int)
bestBlueNodes = np.loadtxt('blueBestSourceNodes.txt', skiprows= 1, dtype= int)

print("Initialize Objects...\n")
# Init graph.
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.DiGraph() )
unGraph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.Graph() ) # Load graph as undirected.
nodes = np.concatenate((randomNodes, bestRedNodes, bestBlueNodes) )

# Write files header. It also clears it in case is already written.
with open('edgeRecScores.txt', 'w') as fileOne:
    fileOne.write('sourceNode\ttargetNode\tnode2vecRecommendationScore\tresourceAllocationScore\t'+
                    'jaccardCoefficientScore\tpreferencialAttachmentScore\tadamicAddarScore\tgain\texpectedGain\n')

# For each source node get their neighbors of maximum distance 3 and
# compute scores. Don't check for random source existing in red or blue
# nodes because it's highly unexpected in large networks.

num = 0
startTime = time.time()
for node in nodes:
    num += 1
    # Estimate total time.
    if num == 10:
        stopTime = time.time()
        ellapsedTime = stopTime - startTime
        print("It will approximatelly need: %f seconds more\n" %((nodes.size / 10) * ellapsedTime) )
    # Get as candidate neighbors all neighbors.
    candidateNeighbors = set([i for i in range(graph.number_of_nodes() )])

    # Remove self from candidate Neighbors.
    candidateNeighbors.remove(node)
    # Remove neighbors from candindate neighbors. 
    for nei in graph.neighbors(node):
        candidateNeighbors.discard(nei)

    if len(candidateNeighbors) != 0:
        # Get candidate edges in a list.
        candidateEdges = [(node, nei) for nei in candidateNeighbors]

        # Compute node2vec recommendation score for edges.
        edgeEmbeddings = fr.EdgeEmbeding.hadamart(candidateEdges)
        edgeRecommendationScore = linkRecommender.predict_proba(edgeEmbeddings)
        edgeRecommendationScore = edgeRecommendationScore[0:,1]

        # Compute Other recommendation scores.
        resourceAllocationPreds = nx.resource_allocation_index(unGraph, candidateEdges)
        jaccardCoefficientPreds = nx.jaccard_coefficient(unGraph, candidateEdges)
        preferencialAttachmentPreds = nx.preferential_attachment(unGraph, candidateEdges)
        adamicAdarPreds = nx.adamic_adar_index(unGraph, candidateEdges)
        
        # Compute gain score.
        gainScore = np.loadtxt('%dedgeFairnessScores.txt' %node, skiprows=1, usecols=1)
        gainPreds = [gainScore[j] for i, j in candidateEdges]

        # convert results to list.
        numberOfEdges = len(candidateEdges)
        resourceAllocationValues = fr.getListOfPreds(resourceAllocationPreds, numberOfEdges)
        jaccardCoefficientValues = fr.getListOfPreds(jaccardCoefficientPreds, numberOfEdges)
        preferencialAttachmentValues = fr.getListOfPreds(preferencialAttachmentPreds, numberOfEdges)
        adamicAdarValues = fr.getListOfPreds(adamicAdarPreds , numberOfEdges)

        # Log these score to "edgeScores.txt" file.
        with open('edgeRecScores.txt', 'a') as fileOne:
            edge = 0
            for source, target in candidateEdges:
                fileOne.write('%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n' 
                            %(source, target, edgeRecommendationScore[edge], resourceAllocationValues[edge], 
                            jaccardCoefficientValues[edge], preferencialAttachmentValues[edge], adamicAdarValues[edge],
                            gainPreds[edge], gainPreds[edge] * edgeRecommendationScore[edge]
                            ) )

                edge += 1
                
for node in nodes:
    subprocess.call(['rm %dedgeFairnessScores.txt' %node], cwd='.', shell=True)