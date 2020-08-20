""" This script runs all the required scriptes and experiments in one
dataset for the FairRec experiments.

It ensures that in each dataset exist all the files that we need
multiple times andneed time. Then it runstheexperiments.
"""
import subprocess
subprocess.call(['cp ~/Workspace/FairRec/Code/PythonFiles/new/fairRec.py .'], cwd= ".", shell= True)
import fairRec as fr
import sys 
import pickle
import time

# --------------------------- Pre Experiment script should have been excuted already --------------------

# ------------------------------------------ Start Experiments ------------------------------------------
# Run experiment one for random sources.
startTime = time.time()
try:
    fileOne = open('redRatioByNode2vecRandomSources.txt', 'r')
    fileOne.close()
except:
    print("Running experiment one for random source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneRandom.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneRandom.out'], cwd=".", shell=True)
print('Experiment one for random source nodes is ready.')
stopTime = time.time()
expOneRandTime = stopTime - startTime

# Greedy only in really small datasets.
fileOne = open('out_graph.txt', 'r')
numberOfNodes = int(fileOne.readline() )
fileOne.close()
startTime = time.time()
if numberOfNodes < 5000:
    try:
        fileOne = open('redRatioByGreedyRandomSources.txt', 'r')
        fileOne.close()
    except:
        print("Running experiment one greedy for random source nodes...")
        subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneGreedyRandom.out .'], cwd= ".", shell= True)
        subprocess.call(['./experimentOneGreedyRandom.out'], cwd=".", shell=True)
    print('Experiment one for greedy random source nodes is ready.')
stopTime = time.time()
expOneGreedyRandTime = stopTime - startTime

# Run experiment one for red sources.
startTime = time.time()
try:
    fileOne = open('redRatioByNode2vecRedSources.txt', 'r')
    fileOne.close()
except:
    print("Running experiment one for red source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneRed.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneRed.out'], cwd=".", shell=True)
print('Experiment one for red source nodes is ready.')
stopTime = time.time()
expOneRedTime = stopTime - startTime

# Greedy only in really small datasets.
fileOne = open('out_graph.txt', 'r')
numberOfNodes = int(fileOne.readline() )
fileOne.close()
startTime = time.time()
if numberOfNodes < 5000:
    try:
        fileOne = open('redRatioByGreedyRedSources.txt', 'r')
        fileOne.close()
    except:
        print("Running experiment one greedy for red source nodes...")
        subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneGreedyRed.out .'], cwd= ".", shell= True)
        subprocess.call(['./experimentOneGreedyRed.out'], cwd=".", shell=True)
print('Experiment one for greedy red source nodes is ready.')
stopTime = time.time()
expOneGreedyRedTime = stopTime - startTime

# Run experiment one for blue sources.
startTime = time.time()
try:
    fileOne = open('redRatioByNode2vecBlueSources.txt', 'r')
    fileOne.close()
except:
    print("Running experiment one for blue source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneBlue.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneBlue.out'], cwd=".", shell=True)
print('Experiment one for blue source nodes is ready.')
stopTime = time.time()
expOneBlueTime = stopTime - startTime

# Greedy only in really small datasets.
fileOne = open('out_graph.txt', 'r')
numberOfNodes = int(fileOne.readline() )
fileOne.close()
startTime = time.time()
if numberOfNodes < 5000:
    try:
        fileOne = open('redRatioByGreedyBlueSources.txt', 'r')
        fileOne.close()
    except:
        print("Running experiment one greedy for blue source nodes...")
        subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneGreedyBlue.out .'], cwd= ".", shell= True)
        subprocess.call(['./experimentOneGreedyBlue.out'], cwd=".", shell=True)
print('Experiment one for greedy blue source nodes is ready.')
stopTime = time.time()
expOneGreedyBlueTime = stopTime - startTime

with open("experimentsTiming.txt", "a") as fileOne:
    fileOne.write("Experiment one except greedy, random sources: %f secodns\n" %expOneRandTime)
    fileOne.write("Experiment one greedy, random sources: %f seconds\n" %expOneGreedyRandTime)
    fileOne.write("Experiment one except greedy, red sources: %f secodns\n" %expOneRedTime)
    fileOne.write("Experiment one greedy, red sources: %f seconds\n" %expOneGreedyRedTime)
    fileOne.write("Experiment one except greedy, blue sources: %f secodns\n" %expOneBlueTime)
    fileOne.write("Experiment one greedy, blue sources: %f seconds\n" %expOneGreedyBlueTime)
    