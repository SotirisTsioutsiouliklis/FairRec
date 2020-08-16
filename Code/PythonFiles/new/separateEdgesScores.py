""" Script to separate the edges scores in appropriate files, depending
on their group. Random, red or blue.
"""
import numpy as np
import pandas as pd
import time

startTime = time.time()
# Get Source Nodes.
randomNodes = np.loadtxt('randomSourceNodes.txt', skiprows= 1, dtype= int)
bestRedNodes = np.loadtxt('redBestSourceNodes.txt', skiprows= 1, dtype= int)
bestBlueNodes = np.loadtxt('blueBestSourceNodes.txt', skiprows= 1, dtype= int)

# Convert to sets for fast searching.
# TODO: Check if convertion is correct from numpy array to set.
# Or if it exists better way. A numpy way.
randomNodes = set(randomNodes)
bestRedNodes = set(bestRedNodes)
bestBlueNodes = set(bestBlueNodes)

# Get edges scores.
edgesScores = pd.read_csv("edgeRecScores.txt", sep='\t')
randomEdgesScores = edgesScores[edgesScores['sourceNode'].isin(randomNodes)]
redEdgesScores = edgesScores[edgesScores['sourceNode'].isin(bestRedNodes)]
blueEdgesScores = edgesScores[edgesScores['sourceNode'].isin(bestBlueNodes)]

# Write new files.
randomEdgesScores.to_csv("edgesScoresRandom.txt", sep= '\t', index = False)
redEdgesScores.to_csv("edgesScoresRed.txt", sep= '\t', index = False)
blueEdgesScores.to_csv("edgesScoresBlue.txt", sep= '\t', index = False)

stopTime = time.time()
ellapsedTime = stopTime - startTime
with open("edgeScoresSeparationTiming.txt", "W") as fileOne:
    fileOne.write("Edges' scores eparation time: %f seconds" %ellapsedTime)