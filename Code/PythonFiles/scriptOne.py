""" Runs in the Synthetic networks the proper experiments to
analyse the behaviour of classic recommenders.

Now participate the experiments 3 and 7.
"""
import sys
import subprocess

categories = ['homophily_0.30_0.30', 'homophily_0.50_0.30', 'homophily_0.70_0.30',
                'homophily_0.30_0.50', 'homophily_0.50_0.50', 'homophily_0.70_0.50',
                'homophily_0.30_0.70', 'homophily_0.50_0.70', 'homophily_0.70_0.70']

for cat in categories:
    print("------------------ homophily: %s --------------------\n" %cat)
    for i in range(10):
        print("---------------------- network: %d ------------------\n" %i)
        tempDirectory = cat + "/network_%d" %i
        # Copy all files needed.
        print("Copy executables...\n")
        subprocess.call(["cp ../../../../Executables/CppFiles/experimentThree.out ."], cwd= tempDirectory , shell=True)
        subprocess.call(["cp ../../../../Executables/CppFiles/experimentThree.out ."], cwd= tempDirectory , shell=True)
        # Run experiments.
        print("Run experiments...\n")
        subprocess.call(["./experimentThree.out"], cwd= tempDirectory , shell=True)
        subprocess.call(["./experimentSeven.out"], cwd= tempDirectory , shell=True)