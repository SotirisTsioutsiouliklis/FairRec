""" This script runs all the required scriptes for experiments in one
dataset for the FairRec experiments.

It ensures that in each dataset exist all the files that we need
multiple times andneed time. Then it runstheexperiments.
"""
import subprocess
subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/fairRec.py .'], cwd= ".", shell= True)
import fairRec as fr
import sys 
import pickle
import time

# Calculate initial pagerank.
startTime = time.time()
try:
    fileOne = open('out_pagerank.txt', 'r')
    fileOne.close()
except:
    print("Pagerank calculation...")
    subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/CppFiles/getPagerank.out .'], cwd= ".", shell= True)
    subprocess.call(['./getPagerank.out'], cwd= ".", shell= True)
print('Pagerank is ready.')
stopTime = time.time()
pagerankTime = stopTime - startTime

# Calculate red absorbing probs.
startTime = time.time()
try:
    fileOne = open('out_personilized_red.txt', 'r')
    fileOne.close()
except:
    print("Red absorbing probs calculation...")
    subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/CppFiles/absorbingProbs.out .'], cwd= ".", shell= True)
    subprocess.call(['./absorbingProbs.out -r'], cwd= ".", shell= True)
print('Reb absorbing probs are ready.')
stopTime = time.time()
redAbsTime = stopTime - startTime

# Get classifier.
startTime = time.time()
try:
    node2vecRecommender = pickle.load(open('edgeClassifier.sav', 'rb') )
except:
    print('Train classifier...')
    fr.trainModel(False)
print('Classifier is ready.')
stopTime = time.time()
trainClassifierTime = stopTime - startTime

# Get Node embeddings.
startTime = time.time()
try:
    fileOne = open('out_nodeEmbeddings.txt', 'r')
    fileOne.close()
except:
    print('Calculate node embeddings...')
    subprocess.call(['cp /mnt/sdb1/tsiou/snap/examples/node2vec/node2vec .'], cwd= ".", shell= True)
    subprocess.call(['./node2vec -i:out_graph.edgelist -o:out_nodeEmbeddings.txt -l:3 -d:128 -p:0.3 -dr -v'], cwd= ".", shell= True)
print('Node embeddings are ready.')
stopTime = time.time()
getEmbTime = stopTime - startTime

# Get source nodes.
startTime = time.time()
try:
    fileOne = open('randomSourceNodes.txt', 'r')
    fileOne.close()
    fileOne = open('redBestSourceNodes.txt', 'r')
    fileOne.close()
    fileOne = open('blueBestSourceNodes.txt', 'r')
    fileOne.close()
except:
    print('Get source nodes...')
    subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/CppFiles/getSourceNodes.out .'], cwd= ".", shell= True)
    subprocess.call(['./getSourceNodes.out'], cwd= ".", shell= True)
print('Source nodes are ready.')
stopTime = time.time()
getSourcesTime = stopTime - startTime

# Get edge fairness score.
startTime = time.time()
try:
    fileOne = open('randomSourceNodes.txt', 'r')
    fileOne.readline()
    testNode = int(fileOne.readline() )
    fileOne.close()
    fileOne = open('%dedgeFairnessScores.txt' %testNode, 'r')
    fileOne.close()
except:
    try:
        fileOne = open("edgeRecScores.txt", "r")
        fileOne.close()
    except:
        try:
            fileONe = open('edgesScoresRandom.txt', 'r')
            fileOne.close()
        except:
            print('Get edge fairness scores...')
            subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/CppFiles/getEdgeFairnessScore.out .'], cwd= ".", shell= True)
            subprocess.call(['./getEdgeFairnessScore.out'], cwd= ".", shell= True)
print('Edge fairness scores are ready.')
stopTime = time.time()
getFairnessScoresTime = stopTime - startTime

# Get edges' scores. if it needs get candidate edges.
startTime = time.time()
fileOne = open('out_graph.txt', 'r')
numberOfNodes = int(fileOne.readline() )
fileOne.close()
if numberOfNodes > 35000:
    pass
    # Get candidate edges.
    # Get edges scores for candidate edges.
else:
    # Get edge scores.
    try:
        fileOne = open('edgeRecScores.txt', 'r')
        fileOne.close()
        # Separate edges.
        print("Edges scores have been calculated all together. Separate edges scores...")
        subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/separateEdgesScores.py .'], cwd= ".", shell= True)
        subprocess.call(['python3 separateEdgesScores.py'], cwd= ".", shell= True)
        subprocess.call(['rm edgeRecScores.txt'], cwd= ".", shell= True)
    except:
        try:
            fileONe = open('edgesScoresRandom.txt', 'r')
            fileOne.close()
        except:
            print('Get edge scores...')
            subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/getEdgesScores.py .'], cwd= ".", shell= True)
            subprocess.call(['python3 getEdgesScores.py'], cwd= ".", shell= True)
print('Edges scores are ready.')
stopTime = time.time()
getEdgesScoresTime = stopTime - startTime

# Get edges distances.
startTime = time.time()
try:
    fileOne = open('edgesDistancesRandomSources.txt', 'r')
except:
    print('Calculate edge distances...')
    subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/getEdgesDistances.py .'], cwd= ".", shell= True)
    subprocess.call(['python3 getEdgesDistances.py'], cwd= ".", shell= True)
print("Edges distances are ready.")
stopTime = time.time()
getCandEdgesDisTime = stopTime - startTime

# Get networks quality features.
startTime = time.time()
try:
    fileONe = open('networksQualityFeatures.txt', 'r')
    fileOne.close()
except:
    print('Calculate Networks quality features...')
    subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/getNetworksQualityFeatures.py .'], cwd= ".", shell= True)
    subprocess.call(['python3 getNetworksQualityFeatures.py'], cwd= ".", shell= True)
print('Networks quality features ready.')
stopTime = time.time()
getQualityNetworkTime = stopTime - startTime

# Get groups quality features.
startTime = time.time()
try:
    fileONe = open('groupQualityFeatures.txt', 'r')
    fileOne.close()
except:
    print('Calculate groups quality features...')
    subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/getGroupsQualityFeatures.py .'], cwd= ".", shell= True)
    subprocess.call(['python3 getGroupsQualityFeatures.py'], cwd= ".", shell= True)
print('Groups quality features are ready.')
stopTime = time.time()
getQualityGroupsTime = stopTime - startTime

# Get nodes quality features.
startTime = time.time()
try:
    fileONe = open('nodeQualityFeatures.txt', 'r')
    fileOne.close()
except:
    print('Calculate nodes quality features...')
    subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/getNodesQualityFeatures.py .'], cwd= ".", shell= True)
    subprocess.call(['python3 getNodesQualityFeatures.py'], cwd= ".", shell= True)
print('Nodes quality features are ready.')
stopTime = time.time()
getQualityNodesTime = stopTime - startTime


with open("preExperimentsTiming.txt", "a") as fileOne:
    fileOne.write("Get initial pagerank: %f seconds\n" %pagerankTime)
    fileOne.write("Get red absorbing probs: %f seconds\n" %redAbsTime)
    fileOne.write("Get classifier: %f seconds\n" %trainClassifierTime)
    fileOne.write("Get node embeddings: %f secodns\n" %getEmbTime)
    fileOne.write("Get source nodes: %f seconds\n" %getSourcesTime)
    fileOne.write("Get fairness scores: %f seconds\n" %getFairnessScoresTime)
    fileOne.write("Get edges scores: %f seconds\n" %getEdgesScoresTime)
    fileOne.write("Get candidate edges distances: %f seconds\n" %getCandEdgesDisTime)
    fileOne.write("Get network's quality features: %f seconds\n" %getQualityNetworkTime)
    fileOne.write("Get groups' quality features: %f seconds\n" %getQualityGroupsTime)
    fileOne.write("Get nodes' quality features: %f seconds\n" %getQualityNodesTime)
