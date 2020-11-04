""" Set the formation of the networks with keep folder.
"""
import subprocess

'''
sizes = [0.5]
homophilies = [0.9]

for size in sizes:
    for homophily in homophilies:
        for network in range(10):
            print('homophily_%.2f_%.2f/network_%d' %(size, homophily, network))
            subprocess.call(['rm *.txt'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            subprocess.call(['cp keep/*.txt .'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
            subprocess.call(['rm -rf keep'], cwd='homophily_%.2f_%.2f/network_%d' %(size, homophily, network), shell= True)
'''

subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/CppFiles/getPagerank.out .'], cwd=".", shell= True)
subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/getNetworksQualityFeatures.py .'], cwd=".", shell= True)
subprocess.call(['cp /mnt/sdb1/tsiou/FairRec/Code/PythonFiles/new/getGroupsQualityFeatures.py .'], cwd=".", shell= True)
subprocess.call(['./getPagerank.out'], cwd=".", shell= True)
subprocess.call(['python3 getNetworksQualityFeatures.py'], cwd=".", shell= True)
subprocess.call(['python3 getGroupsQualityFeatures.py'], cwd=".", shell= True)