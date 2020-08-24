""" Set the formation of thenetworks with keep folder.
"""
import subprocess

sizes = [0.5]
homophilies = [0.9]

for size in sizes:
    for homophily in homophilies:
        for network in range(10):
            print('homophily_%.2f_%.2f/network_%d' %(size, homophily, network))
            subprocess.call(['rm *.txt'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            subprocess.call(['cp keep/*.txt .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            subprocess.call(['rm -rf keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)