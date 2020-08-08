""" This script runs all the required scriptes and experiments in one
dataset for the FairRec experiments.
"""
import subprocess
import sys 

# Calculate red absorbing probs.
print('Take Red absorbing probs...')
try:
    fileOne = open('out_personilized_red.txt', 'r')
    print('Reb absorbing probs already calculated.')
except:
    print("Red absorbing probs calculation...")
    subprocess.call(['cp ~/Workspace/FairRec/Code/CppFiles/absorbingProbs.out .'], cwd= ".", shell= True)
    subprocess.call(['./absorbingProbs.out -r'], cwd= ".", shell= True)

# Get Node embeddings.
print('Get node embeddings...')
try:
    fileOne = open('out_nodeEmbeddings.txt', 'r')
    print('Node embeddings already exists.')
except:
    print('Calculate node embeddings...')
    subprocess.call(['cp ~/Workspace/snap/examples/node2vec/node2vec .'], cwd= ".", shell= True)
    subprocess.call(['./node2vec -i:out_graph.edgelist -o:out_nodeEmbeddings.txt -l:3 -d:128 -p:0.3 -dr -v'], cwd= ".", shell= True)