""" Get recommedation scores for the candidates edges specified
from the input file.

You must specify the file from which the script will read the candidate
edges using the pandas.read_csv(<file_name>,
usecols=[<source nodes column>, <target nodes column>]) and the
recommendation algorthim to use to compute the score for the candidate
edges.

Examples:
    The general syntax is the following:

    >>> python3 getRecommendationScores.py -i <input_file> -p adamic-adar -o <output_file>

    >>> python3 getRecommendationScores.py -i <input_file> -p resource-allocation -o <output_file>

    >>> python3 getRecommendationScores.py -i <input_file> -p jaccard-coefficient -o <output_file>

    >>> python3 getRecommendationScores.py -i <input_file> -p preferential-attachment -o <output_file>

    >>> python3 getRecommendationScores.py -i <input_file> -p fair -o <output_file>

    >>> python3 getRecommendationScores.py -i <input_file> -p from-classifier -c <classifier file> -o <output_file>

    >>> python3 getRecommendationScores.py -i <input_file> -p multiplicative-hybrid -f <fair scores> -c <recommendation scores> -o <output_file>

    >>> python3 getRecommendationScores.py -i candidate_edges_random_10.csv -p adamic-adar -o adamic_adar_10.csv

Notes:
    1. Available policies:
        random, adamic-adar, jaccard-coefficient, preferential-attachment, resource-allocation,
        from-classifier, fair, multiplicative-hybrid.

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
import numpy as np
import pickle
import pandas as pd
import networkx as nx
from random import random
from subprocess import run
from typing import List, Tuple
from sklearn.linear_model import LogisticRegression


class RecommendationPolicies:
    @staticmethod
    def random(edges: np.array) -> List[Tuple[int, int, float]]:
        return [(source, target, random()) for source, target in edges]

    @staticmethod
    def adamicAdar(edges: np.array) -> List[Tuple[int, int, float]]:
        # Initialize graph.
        graph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.Graph())
        preds = nx.adamic_adar_index(graph, edges)
        return [(source, target, score) for source, target, score in preds]

    @staticmethod
    def jaccardCoefficient(edges: np.array) -> List[Tuple[int, int, float]]:
        # Initialize graph.
        graph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.Graph())
        preds = nx.jaccard_coefficient(graph, edges)
        return [(source, target, score) for source, target, score in preds]

    @staticmethod
    def preferentialAttachment(edges: np.array) -> List[Tuple[int, int, float]]:
        # Initialize graph.
        graph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.Graph())
        preds = nx.preferential_attachment(graph, edges)
        return [(source, target, score) for source, target, score in preds]

    @staticmethod
    def resourceAllocation(edges: np.array) -> List[Tuple[int, int, float]]:
        # Initialize graph.
        graph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.Graph())
        preds = nx.resource_allocation_index(graph, edges)
        return [(source, target, score) for source, target, score in preds]

    @staticmethod
    def fromClassifier(edge_file: str, classifier_file: str) -> List[Tuple[int, int, float]]:
        # Load recommender.
        linkRecommender = pickle.load(open(classifier_file, 'rb'))
        # Get edge embeddings.
        edges = pd.read_csv(input_file).to_numpy()
        edgeEmbeddings = edges[:, 2:]
        # Get scores.
        edgeRecommendationScores = linkRecommender.predict_proba(edgeEmbeddings)
        return [(edges[i][0], edges[i][1], edgeRecommendationScores[i][1]) for i in range(len(edgeRecommendationScores))]

    @staticmethod
    def fair(edge_file: str, output_file: str):
        run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/CppFiles/getFairScores.out", "."])
        run(["./getFairScores.out", "-e", f"{edge_file}", "-o", f"{output_file}"])

    @staticmethod
    def multiplicativeHybrid(edges: np.array, fair_scores_file: str, clasiffier_scores_file: str) -> List[Tuple[int, int, float]]:
        fair_scores = pd.read_csv(fair_scores_file)
        clasiffier_scores = pd.read_csv(clasiffier_scores_file)
        hybrid = pd.merge(fair_scores, clasiffier_scores, "inner", on=["Sources", "Targets"])
        hybrid["Scores"] = hybrid["Scores_x"] * hybrid["Scores_y"]
        hybrid.drop("Scores_x", "Scores_y")
        return [(hybrid.iloc[i, 0], hybrid.iloc[i, 1], hybrid.iloc[i, 2]) for i in range(len(hybrid))]


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
            "from-classifier", "fair", "multiplicative-hybrid"]

# Adjust input check according to the new features added.
if __name__ == "__main__":
    #############################
    # Parse command line inputs #
    #############################

    # Check valid number of command line arguments.
    if len(sys.argv) != 7 and len(sys.argv) != 9 and len(sys.argv) != 11:
        InputErrors.argumentError()

    # Check obligatory arguments.
    if len(sys.argv) == 7:
        if sys.argv[1] == "-i" and sys.argv[3] == "-p" and sys.argv[5] == "-o":
            input_file = sys.argv[2]
            policy = sys.argv[4]
            output_file = sys.argv[6]
            # Check valid policy.
            if policy not in policies:
                InputErrors.valueError()
        else:
            InputErrors.argumentError()
    elif len(sys.argv) == 9:
        if sys.argv[1] == "-i" and sys.argv[3] == "-p" and sys.argv[5] == "-c" and sys.argv[7] == "-o":
            input_file = sys.argv[2]
            policy = sys.argv[4]
            classifier_file = sys.argv[6]
            output_file = sys.argv[8]
            # Check valid policy.
            if policy not in policies:
                InputErrors.valueError()
        else:
            InputErrors.argumentError()
    elif len(sys.argv) == 11:
        if sys.argv[1] == "-i" and sys.argv[3] == "-p" and sys.argv[5] == "-f" and sys.argv[7] == "-c" and sys.argv[7] == "-o":
            input_file = sys.argv[2]
            policy = sys.argv[4]
            fair_scores_file = sys.argv[6]
            classifier_score_file = sys.argv[8]
            output_file = sys.argv[10]
            # Check valid policy.
            if policy not in policies:
                InputErrors.valueError()
        else:
            InputErrors.argumentError()

    # Load candidate edges.
    edges = pd.read_csv(input_file, header=0, names=["Sources", "Targets"]).to_numpy()

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
    elif policy == "from-classifier":
        scores = RecommendationPolicies.fromClassifier(input_file, classifier_file)
    elif policy == "fair":
        RecommendationPolicies.fair(input_file, output_file)
    elif policy == "multiplicative-hybrid":
        scores = RecommendationPolicies.multiplicativeHybrid(edges, fair_scores_file, classifier_score_file, output_file)
    else:
        InputErrors.valueError()

    # Save scores.
    if policy != "fair":
        scores = pd.DataFrame(scores, columns=["Sources", "Targets", "Scores"])
        scores.to_csv(output_file, index=False)
