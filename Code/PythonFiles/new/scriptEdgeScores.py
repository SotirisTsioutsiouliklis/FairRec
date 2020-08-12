""" Automatization script to get the edges' scores.
"""
import subprocess
import sys

datasets = ['blogs', 'tmdb', 'twitter', 'physics', 'dblp_aminer', 'linkedin']

#'homophily_0.10_0.10', 'homophily_0.30_0.10', 'homophily_0.50_0.10', 
directories = ['homophily_0.10_0.10', 'homophily_0.10_0.30', 'homophily_0.10_0.50', 'homophily_0.10_0.70', 'homophily_0.10_0.90',
                'homophily_0.30_0.10', 'homophily_0.30_0.30', 'homophily_0.30_0.50', 'homophily_0.30_0.70', 'homophily_0.30_0.90',
                'homophily_0.50_0.10', 'homophily_0.50_0.30', 'homophily_0.50_0.50', 'homophily_0.50_0.70', 'homophily_0.50_0.90']

if len(sys.argv) != 2:
    sys.exit("Options:\n\tpython3 exp<Number>.py -s : For synthetic datasets\n\tpython3 exp<Number>.py -r : For real datasets")
else:
    if sys.argv[1] == '-s':
        for directory in directories:
            for i in range(10):
                print("In directory %s, in network %d\n" %(directory, i) )
                #subprocess.call(["cp ../Executables/PythonFiles/getEdgesScores.py %s/network_%d" %(directory, i)], cwd=".", shell=True)
                #subprocess.call(["cp ../Executables/CppFiles/getSourceNodes.out %s/network_%d" %(directory, i)], cwd=".", shell=True)
                #subprocess.call(["cp ../Executables/CppFiles/getEdgeFairnessScore.out %s/network_%d" %(directory, i)], cwd=".", shell=True)
                #subprocess.call(["cp ../../snap/examples/node2vec/node2vec %s/network_%d" %(directory, i)], cwd=".", shell=True)
                #subprocess.call(["./getSourceNodes.out"], cwd= "%s/network_%d" %(directory, i), shell=True)
                #subprocess.call(["python3 getEdgesScores.py"], cwd= "%s/network_%d" %(directory, i), shell=True)
                subprocess.call(['cp ~/Workspace/FairRec/Code/PythonFiles/new/temp.py %s/network_%d' %(directory, i)], cwd= ".", shell= True)
                subprocess.call(["python3 temp.py"], cwd= "%s/network_%d" %(directory, i), shell= True)
    elif sys.argv[1] == '-r':
        for dataset in datasets:
            print("In dataset: ", dataset)
            subprocess.call(["cp ../Executables/PythonFiles/getEdgesScores.py %s" %dataset], cwd=".", shell=True)
            subprocess.call(["python3 getEdgesScores.py"], cwd= "%s" %dataset, shell=True)
    else:
        sys.exit("Options:\n\tpython3 exp<Number>.py -s : For synthetic datasets\n\tpython3 exp<Number>.py -r : For real datasets")