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
            print('homophily_%.2f_%.2f/network_%d' %(size, homophily, network))
            print('Delete previous, if any.')
            subprocess.call(['mkdir keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['cp out_graph.txt keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['cp out_community.txt keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['rm *.txt'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['rm *.sav'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['rm *.py'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['rm *.out'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['rm node2vec'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['cp keep/*.txt .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            subprocess.call(['rm -rf keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell = True)
            print('Run scripts.')
            subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/scriptAllToOne.py .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            subprocess.call(['python3 scriptAllToOne.py'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
symStopTime = time.time()
symEllapsedTime = symStopTime - symStartTime
print("Time for all synthetic symmetric (score edges pre calculated, not separated): %f seconds" %symEllapsedTime)