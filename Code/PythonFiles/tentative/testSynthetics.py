""" Test which of the synthetics have run properly.
"""
import subprocess
import sys


sizes = [0.1, 0.3, 0.5]
homophilies = [0.1, 0.3, 0.5, 0.7, 0.9]

for size in sizes:
    for homophily in homophilies:
        for network in range(10):
            print('homophily_%.2f_%.2f/network_%d' %(size, homophily, network) )
            #subprocess.call(['mkdir keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['cp out_graph.txt keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['cp out_community.txt keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['rm *.txt'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['cp keep/*.txt .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['rm -rf keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            try:
                subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/CppFiles/absorbingProbs.out .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
                subprocess.call(['./absorbingProbs.out -r'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            except:
                print('Problem Pagerank')
                sys.exit()

for size in sizes:
    for homophily in homophilies:
        for network in range(10):
            print('Sensitive - homophily_%.2f_%.2f/network_%d' %(size, homophily, network) )
            #subprocess.call(['mkdir keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['cp out_graph.txt keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['cp out_community.txt keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['rm *.txt'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['cp keep/*.txt .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            #subprocess.call(['rm -rf keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            try:
                subprocess.call(['cp /mnt/sdb1/tsiou/FairLaR/Code/Python_files/sensitive.py .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
                #subprocess.call(['python3 sensitive.py 0'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            except:
                print('Problem Sensitive')
                sys.exit()
