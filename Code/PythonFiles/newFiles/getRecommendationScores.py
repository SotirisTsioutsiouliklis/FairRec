""" Get recommedation scores for the candidates edges specified
from the input file.

You must specify the file from which the script will read the candidate
edges using the pandas.read_csv(<file_name>,
usecols=[<source nodes column>, <target nodes column>]) and the
recommendation algorthim to use to compute the score for the candidate
edges.

Examples:
    The general syntax is the following:

    >>> python3 getRecommendationScores.py -i <input_file> -c <source column name> <target column name> -p <recommendation policy> -o <output_file>

    >>> python3 getRecommendationScores.py -i candidate_edges_random_10.csv -c Sources Targets -p adamic-adar -o adamic_adar_10.csv

Notes:
    1. Available policies:
        random, adamic-adar, jaccard-coefficient, preferential-attachment, resource-allocation,
        node2vec, fairwalk, fair, hybrid-node2vec.

    2. Dependencies:
        a. node2vec:
            i. Edge embeddings.
            ii. Trained classifier.

        b. fairwalk:
            i. Edge embeddings.
            ii. Trained classifier.

        c. fair:
            i. Edge fair scores.

        d. hybrid-node2vec:
            i. Edge fair scores.
            ii. Edge embeddings.
            iii. Trained classifier.

TODO: Implement dependency error. Useful, for example, to fair policy which need
    the precomputed scores or to node2vec which needs the classifier.
"""
import sys
import pandas as pd
import networkx as nx
from typing import List, Tuple


class RecommendationPolicies:
    @staticmethod
    def random(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass

    @staticmethod
    def adamicAdar(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass

    @staticmethod
    def jaccardCoefficient(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass

    @staticmethod
    def preferentialAttachment(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass

    @staticmethod
    def resourceAllocation(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass

    @staticmethod
    def node2vec(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass

    @staticmethod
    def fairwalk(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass

    @staticmethod
    def fair(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass

    @staticmethod
    def hybridNode2vec(edges: List[Tuple[int, int]]) -> List[Tuple[int, int, float]]:
        pass


# Adjust print message according to the new features added.
class InputErrors:
    @staticmethod
    def argumentError():
        """ Terminates script due to input error and prints message
        with use instructions.
        """
        sys.exit("ERROR! No valid command line arguments\n\
                use:\n\
                python3 getRecommendationScores.py -i <input_file> -c <source column name> <target column name> -p <recommendation policy> -o <output_file>\n\
                e.g.\n\
                >>> python3 getRecommendationScores.py -i candidate_edges_random_10.csv -c Sources Targets -p adamic-adar -o adamic_adar_10.csv\n\n\
                    acceptable policies:\n\
                    random, adamic-adar, jaccard-coefficient, preferential-attachment, resource-allocation,\
                    node2vec, fairwalk, fair, hybrid-node2vec")

    @staticmethod
    def valueError():
        """ Terminates script and print message for acceptable
        values.
        """
        sys.exit("Error! Policy not valid.\n\
                 acceptable policies:\n\
                 random, adamic-adar, jaccard-coefficient, preferential-attachment, resource-allocation,\
                 node2vec, fairwalk, fair, hybrid-node2vec")

    @staticmethod
    def dependencyError(algorithm, dependency):
        pass


# Valid recommendation policies.
policies = ["random", "adamic-adar", "jaccard-coefficient", "preferential-attachment", "resource-allocation",
            "node2vec", "fairwalk", "fair", "hybrid-node2vec"]

# Adjust input check according to the new features added.
if __name__ == "__main__":
    #############################
    # Parse command line inputs #
    #############################

    # Check valid number of command line arguments.
    if not len(sys.argv) != 10:
        InputErrors.argumentError()

    # Check obligatory arguments.
    if sys.argv[1] == "-i" and sys.arv[3] == "-c" and sys.argv[6] == "-p" and sys.argv[8] == "-o":
        input_file = sys.argv[2]
        coloumns = [sys.argv[4], sys.argv[5]]  # First is Sources, second is Targets.
        policy = sys.argv[7]
        output_file = sys.argv[9]
        # Check valid policy.
        if policy not in policies:
            InputErrors.valueError()
    else:
        InputErrors.argumentError()

    # Load candidate edges.
    edges = pd.read_csv(input_file, coloumns=coloumns, header=0, names=["Sources", "Targets"])

    # Get recommendation scores.
    if policy == "random":
        scores = RecommendationPolicies.random(edges)
    elif policy == "adamic-adar":
        scores = RecommendationPolicies.adamicAdar(edges)
    elif policy == "jaccard-coefficient":
        scores = RecommendationPolicies.jaccardCoefficient(edges)
    elif policy == "preferential-attachment":
        scores = RecommendationPolicies.preferentialAttachment(edges)
    elif policy == "resource-allocation":
        scores = RecommendationPolicies.resourceAllocation(edges)
    elif policy == "node2vec":
        scores = RecommendationPolicies.node2vec(edges)
    elif policy == "fairwalk":
        scores = RecommendationPolicies.fairwalk(edges)
    elif policy == "fair":
        scores = RecommendationPolicies.fair(edges)
    elif policy == "hybrid-node2vec":
        scores = RecommendationPolicies.hybridNode2vec(edges)
    else:
        InputErrors.valueError()

    # Save scores.
    scores.to_csv(output_file, index=False)
