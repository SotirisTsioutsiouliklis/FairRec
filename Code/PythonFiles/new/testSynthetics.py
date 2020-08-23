""" Test which of the synthetics have run properly.
"""
import subprocess


sizes = [0.5]
homophilies = [0.7, 0.9]

for size in sizes:
    for homophily in homophilies:
        for network in range(10):
            print('homophily_%.2f_%.2f/network_%d' %(size, homophily, network))
            subprocess.call(['mkdir keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            subprocess.call(['cp out_graph.txt keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            subprocess.call(['cp out_community.txt keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            try:
                subprocess.call(['cp ~/Workspace/FairRec/Code/PythonFiles/new/scriptAllToOne.py keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
                subprocess.call(['python3 scriptAllToOne.py'], cwd='homophily_%.2f_%.2f/network_%d/keep' %(size, homophily, network), shell= True)
            except:
                print('Problem')



