""" This script runs all the required scriptes for experiments analysis
in one dataset for the FairRec experiments.

It ensures that in each dataset exist all the files that we need
multiple times andneed time. Then it runstheexperiments.
"""
import subprocess
subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/fairRec.py .'], cwd= ".", shell= True)
import fairRec as fr
import sys 
import pickle
import time

# Get selected edges distances.
startTime = time.time()
try:
    fileOne = open("selectedEdgesByNode2vecRandomSourcesDistances.txt", "r")
    fileOne.close()
except:
    print('Calculate selected edges distances...')
    subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/getSelectedEdgesDistances.py .'], cwd= ".", shell= True)
    subprocess.call(['python3 getSelectedEdgesDistances.py'], cwd= ".", shell= True)
print("Selected edges distances ready.")
stopTime = time.time()
selEdgeDisTime = stopTime - startTime

with open("postExperimentsTiming.txt", "a") as fileOne:
    fileOne.write("Get selected edges distances: %f seconds\n" %selEdgeDisTime)
