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
# Run experiment one.
print('Run experiment one...')
try:
    fileOne = open('redRatioByNode2vecRandom.txt', 'r')
    fileOne.close()
    print('Experiment one for random source nodes is ready.')
except:
    print("Running experiment one for random source nodes...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/experimentOne.out .'], cwd= ".", shell= True)
    subprocess.call(['./experimentOne.out'], cwd=".", shell=True)

# Run experiment Two.

