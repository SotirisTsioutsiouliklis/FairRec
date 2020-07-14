""" Runs experiments to datasets in list.

You must run it inside the fairnessInRecommendation directory.
"""
import subprocess
import sys

datasets = ['blogs', 'tmdb', 'twitter', 'physics', 'dblp_aminer', 'linkedin']

directories = ['homophily_0.30_0.30', 'homophily_0.50_0.30', 'homophily_0.70_0.30',
                'homophily_0.30_0.50', 'homophily_0.50_0.50', 'homophily_0.70_0.50',
                'homophily_0.30_0.70', 'homophily_0.50_0.70', 'homophily_0.70_0.70']

if len(sys.argv) != 3:
    sys.exit("Options:\n\tpython3 exp<Number>.py -s : For synthetic datasets\n\tpython3 exp<Number>.py -r : For real datasets")
else:
    if sys.argv[2] == '-s':
        for directory in directories:
            for i in range(10):
                print("In directory %s, in network %d\n" %(directory, i) )
                subprocess.call(["cp ../../Executables/CppFiles/experimentSeven.out %s/network_%d" %(directory, i)], cwd=".", shell=True)
                subprocess.call(["./experimentSeven.out"], cwd= "%s/network_%d" %(directory, i), shell=True)
    elif sys.argv[2] == '-r':
        for dataset in datasets:
                # Copy Executables.
                print("In dataset: ", dataset)
                subprocess.call("cp ../Executables/CppFiles/experimentSeven.out %s" %dataset, cwd=".", shell=True)
                # Execute it.
                subprocess.call("./experimentSeven.out", cwd="%s" %dataset, shell=True)
    else:
        sys.exit("Options:\n\tpython3 exp<Number>.py -s : For synthetic datasets\n\tpython3 exp<Number>.py -r : For real datasets")
        