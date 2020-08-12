""" Gets scores for candidate edges.

@NOTE: Let script run all to one do the job.
@TODO: Update documentation.

Gets the following scores for edges that doesn't exist in the graph
for each of the source node group that we have already calculate. See
repository's README for exact definition of the scores below.

Create file:
    1. edgeRecScores<TypeOfSourceNodes>.txt: <sourceNode>\t<targetNode>\t<node2vecRecommendationScore>\t<resourceAllocationScore>\t
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

def getEdgesScores(fileName, nodes):
    global linkRecommender
    global graph
    global unGraph

    # Write files header. It also clears it in case is already written.
    with open(fileName, 'w') as fileOne:
        fileOne.write('sourceNode\ttargetNode\tnode2vecRecommendationScore\tresourceAllocationScore\t'+
                        'jaccardCoefficientScore\tpreferencialAttachmentScore\tadamicAddarScore\tgain\texpectedGain\n')

    for node in nodes:
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
            with open(fileName, 'a') as fileOne:
                edge = 0
                for source, target in candidateEdges:
                    fileOne.write('%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n' 
                                %(source, target, edgeRecommendationScore[edge], resourceAllocationValues[edge], 
                                jaccardCoefficientValues[edge], preferencialAttachmentValues[edge], adamicAdarValues[edge],
                                gainPreds[edge], gainPreds[edge] * edgeRecommendationScore[edge]
                                ) )

                    edge += 1

# --------------------------------------------------------------
# Load recommender.
linkRecommender = pickle.load(open('edgeClassifier.sav', 'rb') )

# Get Source Nodes.
randomNodes = np.loadtxt('randomSourceNodes.txt', skiprows= 1, dtype= int)
bestRedNodes = np.loadtxt('redBestSourceNodes.txt', skiprows= 1, dtype= int)
bestBlueNodes = np.loadtxt('blueBestSourceNodes.txt', skiprows= 1, dtype= int)

# Init graph.
print("Initialize Objects...\n")
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.DiGraph() )
unGraph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.Graph() ) # Load graph as undirected.
nodes = np.concatenate((randomNodes, bestRedNodes, bestBlueNodes) )

startTime = time.time()

# Add score calculations here...
getEdgesScores("edgesScoresRandom.txt", randomNodes)
getEdgesScores("edgesScoresRed.txt", bestRedNodes)
getEdgesScores("edgesScoresBlue.txt", bestBlueNodes)

stopTime = time.time()
elapsedTime = stopTime - startTime

with open("edgesScoreCalculationTiming.txt", 'w') as fileOne:
    fileOne.write("Total time for calculation of all edges score: %f seconds\n" %elapsedTime)

# Remove add hoch files.          
for node in nodes:
    subprocess.call(['rm %dedgeFairnessScores.txt' %node], cwd='.', shell=True)