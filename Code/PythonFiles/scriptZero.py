""" Runs experiments to datasets in list.

You must run it inside the fairnessInRecommendation directory.
"""
import subprocess

datasets = ['blogs', 'tmdb', 'twitter', 'physics', 'dblp_aminer'] #'blogs', 

for dataset in datasets:
    # Copy Executables.
    print(dataset)
    subprocess.call("cp ../Executables/CppFiles/experimentZero.out %s" %dataset, cwd=".", shell=True)
    subprocess.call("cp ../Executables/CppFiles/experimentTwo.out %s" %dataset, cwd=".", shell=True)
    # Execute it.
    subprocess.call("./experimentZero.out", cwd="%s" %dataset, shell=True)
    subprocess.call("./experimentTwo.out", cwd="%s" %dataset, shell=True)