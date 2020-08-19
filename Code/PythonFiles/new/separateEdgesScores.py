""" Script to separate the edges scores in appropriate files, depending
on their group. Random, red or blue.
"""
import subprocess
import numpy as np
import pandas as pd
import time
import sys

def readScores(fileName, sourceNodes):
    """ Pending...
    """
    edgesScores = dict()

    # Init dictionary.
    for node in sourceNodes:
        edgesScores[node] = dict()

    with open(fileName, 'r') as fileOne:
        fileOne.readline()
        for line in fileOne:
            info = line.split()
            sourceNode = int(info[0]) # For key convention in dict edgesScores.
            targetNode = info[1]
            node2vec = info[2]
            resourceAllocation = info[3]
            jaccardCoef = info[4]
            prefAttach = info[5]
            addamicAdar = info[6]
            gain = info[7]
            expGain = info[8]
    
            edgesScores[sourceNode][targetNode] = dict()
            edgesScores[sourceNode][targetNode]['node2vec'] = node2vec
            edgesScores[sourceNode][targetNode]['resourceAllocation'] = resourceAllocation
            edgesScores[sourceNode][targetNode]['jaccardCoeffficient'] = jaccardCoef
            edgesScores[sourceNode][targetNode]['prefferentialAttachment'] = prefAttach
            edgesScores[sourceNode][targetNode]['addamicAdar'] = addamicAdar
            edgesScores[sourceNode][targetNode]['gain'] = gain
            edgesScores[sourceNode][targetNode]['expectedGain'] = expGain
    
    return edgesScores

def storeEdgesInGroup(fileName, sourceNodesSet, edgesScores):
    with open(fileName, "w") as fileOne:
        fileOne.write('sourceNode\ttargetNode\tnode2vecRecommendationScore\tresourceAllocationScore\t'+
                        'jaccardCoefficientScore\tpreferencialAttachmentScore\tadamicAddarScore\tgain\texpectedGain\n')
        for sourceNode in edgesScores:
            if sourceNode in sourceNodesSet:
                for targetNode in edgesScores[sourceNode]:
                    fileOne.write('%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' %(sourceNode, targetNode, edgesScores[sourceNode][targetNode]['node2vec'],
                       edgesScores[sourceNode][targetNode]['resourceAllocation'], edgesScores[sourceNode][targetNode]['jaccardCoeffficient'],
                       edgesScores[sourceNode][targetNode]['prefferentialAttachment'], edgesScores[sourceNode][targetNode]['addamicAdar'],
                       edgesScores[sourceNode][targetNode]['gain'], edgesScores[sourceNode][targetNode]['expectedGain']) )

startTime = time.time()
# Get Source Nodes.
randomNodes = np.loadtxt('randomSourceNodes.txt', skiprows= 1, dtype= int)
bestRedNodes = np.loadtxt('redBestSourceNodes.txt', skiprows= 1, dtype= int)
bestBlueNodes = np.loadtxt('blueBestSourceNodes.txt', skiprows= 1, dtype= int)

# Convert to sets for fast searching.
sourceNodes = set(np.concatenate((randomNodes, bestRedNodes, bestBlueNodes) ) )
randomNodes = set(randomNodes)
bestRedNodes = set(bestRedNodes)
bestBlueNodes = set(bestBlueNodes)

# Get edges scores.
edgesScores = readScores("edgeRecScores.txt", sourceNodes) # Dict of dicts.
storeEdgesInGroup("edgesScoresRandom.txt", randomNodes, edgesScores)
storeEdgesInGroup("edgesScoresRed.txt", bestRedNodes, edgesScores)
storeEdgesInGroup("edgesScoresBlue.txt", bestBlueNodes, edgesScores)

stopTime = time.time()
ellapsedTime = stopTime - startTime
with open("edgeScoresSeparationTiming.txt", "w") as fileOne:
    fileOne.write("Edges' scores separation time: %f seconds" %ellapsedTime)

# Remove add hoch files. 
nodes = np.concatenate((randomNodes, bestRedNodes, bestBlueNodes) )         
for node in nodes:
    subprocess.call(['rm %dedgeFairnessScores.txt' %node], cwd='.', shell=True)