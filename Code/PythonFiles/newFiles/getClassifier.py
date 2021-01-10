""" Creates a classifier to be used for recommendations.

We define the graph file, the negative edge sample and the output file.

Examples:

    >>> python3 getClassifier.py -g <graph file> -n <negative edge sample file> -o <output file>

    >>> python3 getClassifier.py -g out_graph.txt -n negative_edge_sample.csv -o node2vec_recommender.sav

Notes:
"""
import sys
import pickle
import numpy as np
import pandas as pd
from sklearn.linear_model import LogisticRegression


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
    if (not len(sys.argv) != 7):
        InputErrors.argumentError()

    # Check obligatory arguments.
    if sys.argv[1] == "-g" and sys.argv[3] == "-n" and sys.argv[5] == "-o":
        positive_edge_file = sys.argv[2]
        negative_edge_file = sys.argv[4]
        output_file = sys.argv[6]

    # Create X, Y.
    positive_sample = pd.read_csv(positive_edge_file).drop(columns=["Sources", "Targets"], axis=1).to_numpy()
    negative_sample = pd.read_csv(negative_edge_file).drop(columns=["Sources", "Targets"], axis=1).to_numpy()
    X = np.concatenate((positive_sample, negative_sample))
    Y = np.concatenate(([1 for i in range(len(positive_sample))], [0 for i in range(len(negative_sample))]))

    # Fit the model.
    clf = LogisticRegression()
    clf.fit(X, Y)

    # Save the model.
    pickle.dump(clf, open(output_file, 'wb'))
