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

# --------------------------- Pre Experiment script should have been excuted already --------------------

# ------------------------------------------ Start Experiments ------------------------------------------
# Run experiment one for random sources.
print('Run experiment one for radom sources...')
try:
    fileOne = open('redRatioByNode2vecRandomSources.txt', 'r')
    fileOne.close()
    print('Experiment one for random source nodes is ready.')
except:
    print("Running experiment one for random source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneRandom.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneRandom.out'], cwd=".", shell=True)

try:
    fileOne = open('redRatioByGreedyRandomSources.txt', 'r')
    fileOne.close()
    print('Experiment one for greedy random source nodes is ready.')
except:
    print("Running experiment one greedy for random source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneGreedyRandom.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneGreedyRandom.out'], cwd=".", shell=True)

# Run experiment one for red sources.
print('Run experiment one for red sources...')
try:
    fileOne = open('redRatioByNode2vecRedSources.txt', 'r')
    fileOne.close()
    print('Experiment one for red source nodes is ready.')
except:
    print("Running experiment one for red source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneRed.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneRed.out'], cwd=".", shell=True)

try:
    fileOne = open('redRatioByGreedyRedSources.txt', 'r')
    fileOne.close()
    print('Experiment one for greedy red source nodes is ready.')
except:
    print("Running experiment one greedy for red source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneGreedyRed.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneGreedyRed.out'], cwd=".", shell=True)

# Run experiment one for blue sources.
print('Run experiment one for blue sources...')
try:
    fileOne = open('redRatioByNode2vecBlueSources.txt', 'r')
    fileOne.close()
    print('Experiment one for blue source nodes is ready.')
except:
    print("Running experiment one for blue source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneBlue.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneBlue.out'], cwd=".", shell=True)

try:
    fileOne = open('redRatioByGreedyBlueSources.txt', 'r')
    fileOne.close()
    print('Experiment one for greedy blue source nodes is ready.')
except:
    print("Running experiment one greedy for blue source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOneGreedyBlue.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOneGreedyBlue.out'], cwd=".", shell=True)

