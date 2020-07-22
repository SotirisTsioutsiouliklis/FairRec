""" It copies useful synthetics with useful files to new directory.
"""
import subprocess
import sys

directories = ['homophily_0.10_0.10', 'homophily_0.30_0.10', 'homophily_0.50_0.10', 'homophily_0.70_0.10', 'homophily_0.90_0.10',
                'homophily_0.10_0.30', 'homophily_0.30_0.30', 'homophily_0.50_0.30', 'homophily_0.70_0.30', 'homophily_0.90_0.30',
                'homophily_0.10_0.50', 'homophily_0.30_0.50', 'homophily_0.50_0.50', 'homophily_0.70_0.50', 'homophily_0.90_0.50',]

oldDirectory = 'syntheticSymmetric'
newDirectory = 'synthetics'

# Create new directory.
subprocess.call(['mkdir %s' %newDirectory], cwd= '.', shell=True)

for directory in directories:
    # Create category's subdirectory.
    subprocess.call(['mkdir %s' %directory], cwd= '%s' %newDirectory, shell=True)
    # For each network.
    for i in range(10):
        # Create network's subdirectory.
        subprocess.call(['mkdir network_%d' %i], cwd='%s/%s' %(newDirectory, directory), shell=True)
        # Copy useful files.
        subprocess.call(['cp %s/%s/network_%d/out_graph.txt %s/%s/network_%d' %(oldDirectory, directory, i, newDirectory, directory, i)], cwd=".", shell=True)
        subprocess.call(['cp %s/%s/network_%d/out_community.txt %s/%s/network_%d' %(oldDirectory, directory, i, newDirectory, directory, i)], cwd=".", shell=True)
        try:
            subprocess.call(['cp %s/%s/network_%d/edgeClassifier.sav %s/%s/network_%d' %(oldDirectory, directory, i, newDirectory, directory, i)], cwd=".", shell=True)
        except:
            print('No classifeir found')
        try:
            subprocess.call(['cp %s/%s/network_%d/out_nodeEmbeddings.txt %s/%s/network_%d' %(oldDirectory, directory, i, newDirectory, directory, i)], cwd=".", shell=True)
        except:
            print("No embeddings found")