""" Get the red personilized pagerank for all the nodes in the network.

We define the graph file and the output file.

Examples:

    >>> python3 getRedAbsorbingProbs.py -g <graph file> -o <output file>

    >>> python3 getClassifier.py -g out_graph.txt -o node2vec_recommender.sav

Notes:
"""
import sys
import pandas as pd
from subprocess import run


# Adjust print message according to the new features added.
class InputErrors:
    @staticmethod
    def argumentError():
        """ Terminates script due to input error and prints message
        with use instructions.
        """
        sys.exit("ERROR! No valid command line arguments\n\
                use:\n\
                >>> python3 getClassifier.py -p <negative edge embeddings file> -n <negative edge embeddings file> -o <output file>\n\
                e.g.\n\
                >>> python3 getClassifier.py -p positive_edge_sample.csv -n negative_edge_sample.csv -o node2vec_recommender.sav")

    @staticmethod
    def valueError():
        """ Terminates script and print message for acceptable
        values.
        """
        sys.exit("Error! Not valid percentage.\n\
                 0 < percentage <= 100")

    @staticmethod
    def dependencyError(algorithm, dependency):
        pass


# Adjust input check according to the new features added.
if __name__ == "__main__":
    #############################
    # Parse command line inputs #
    #############################

    # Check valid number of command line arguments.
    if (len(sys.argv) != 5):
        InputErrors.argumentError()

    # Check obligatory arguments.
    if sys.argv[1] == "-g" and sys.argv[3] == "-o":
        graph_file = sys.argv[2]
        output_file = sys.argv[4]

    # Copy & run executable.
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/CppFiles/absorbingProbs.out", "."])
    run(["./absorbingProbs.out", "-r"])

    # Convert result to pandas DataFrame.
    df = pd.read_csv("out_personilized_red.txt", sep="\t", header=0, names=["Nodes", "Red Pagerank"])
    # Delete old file.
    run(["rm", "out_personilized_red.txt"])
    df.to_csv(output_file, index=False)
