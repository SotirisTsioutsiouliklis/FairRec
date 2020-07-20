""" It copies useful sreal with useful files to new directory.
"""
import subprocess
import sys

directories = ['blogs', 'tmdb', 'twitter', 'physics', 'dblp_aminer', 'linkedin']

oldDirectory = '.'
newDirectory = 'real'

# Create new directory.
subprocess.call(['mkdir %s' %newDirectory], cwd= '.', shell=True)

for directory in directories:
    # Create category's subdirectory.
    subprocess.call(['mkdir %s' %directory], cwd= '%s' %newDirectory, shell=True)
    # Copy useful files.
    subprocess.call(['cp %s/%s/out_graph.txt %s/%s' %(oldDirectory, directory, newDirectory, directory)], cwd=".", shell=True)
    subprocess.call(['cp %s/%s/out_community.txt %s/%s' %(oldDirectory, directory, newDirectory, directory)], cwd=".", shell=True)
    subprocess.call(['cp %s/%s/edgeClassifier.sav %s/%s' %(oldDirectory, directory, newDirectory, directory)], cwd=".", shell=True)
    subprocess.call(['cp %s/%s/out_nodeEmbeddings.txt %s/%s' %(oldDirectory, directory, newDirectory, directory)], cwd=".", shell=True)