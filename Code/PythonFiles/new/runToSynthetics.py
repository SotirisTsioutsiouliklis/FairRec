""" Pending...
"""
import subprocess
import time

sizes = [0.1, 0.3, 0.5]
homophilies = [0.1, 0.3, 0.5, 0.7, 0.9]

symStartTime= time.time()
for size in sizes:
    for homophily in homophilies:
        for network in range(10):
            subprocess.call(['cp ~/Workspace/FairRec/Code/PythonFiles/new/scriptAllToOne.py .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            subprocess.call(['python3 scriptAllToOne.py'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
symStopTime = time.time()
symEllapsedTime = symStopTime - symStartTime
print("Time for all synthetic symmetric (score edges pre calculated, not separated): %f seconds" %symEllapsedTime)