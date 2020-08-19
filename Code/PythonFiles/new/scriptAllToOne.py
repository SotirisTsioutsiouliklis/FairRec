""" This script runs all the required scriptes for experiments in one
dataset for the FairRec experiments.

It ensures that in each dataset exist all the files that we need
multiple times andneed time. Then it runstheexperiments.
"""
import subprocess

# Pre experiments.
try:
    subprocess.call(['cp ~/Workspace/FairRec/Code/PythonFiles/new/preExperimentsScript.py .'], cwd= ".", shell= True)
    subprocess.call(['python3 preExperimentsScript.py'], cwd= ".", shell= True)
except:
    print("Pre experiments script didn't run successfully.")
    
# Experiments.
try:
    subprocess.call(['cp ~/Workspace/FairRec/Code/PythonFiles/new/experimentsScript.py .'], cwd= ".", shell= True)
    subprocess.call(['python3 experimentsScript.py'], cwd= ".", shell= True)
except:
    print("Experiments script didn't run successfully.")

# Post experiments.
# Pre experiments.
try:
    subprocess.call(['cp ~/Workspace/FairRec/Code/PythonFiles/new/postExperimentsScript.py .'], cwd= ".", shell= True)
    subprocess.call(['python3 postExperimentsScript.py'], cwd= ".", shell= True)
except:
    print("Pre experiments script didn't run successfully.")


