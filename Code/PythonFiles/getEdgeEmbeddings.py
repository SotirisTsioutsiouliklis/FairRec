""" Get edge embeddings.

specify a node embedding file and get the node embeddings and an embedding
policy.

Examples:
    the general syntax is the following:

        >>> python3 getEdgeEmbeddings.py -i <node embedding file> -e <edge file> -p <edge embedding policy> -o <output file>

        >>> python3 getEdgeEmbeddings.py -i random_node2vec_node_embeddings.csv -e candidate_edges_random_10.csv -p hadamart -o random_node2vec_edge_embeddings.csv

Notes:
    1. Available policies:
        hadamart, concatenate.

    2. Node embeddings file should follow specific conventions. For details
    see getNodeEmbeddings.py

    3. Dependencies:
        a. node2vec node embeddings file.
        b. fairwalk node embeddings file.
TODO:
    1. Add the ability to define embedding arguments.
    2. Add location to fairwalk executable.
    3. Add dependency errors and fix error messages.
"""
import sys
import numpy as np
import pandas as pd
from typing import List, Tuple


class EdgeEmbeding:
    """ Computes the edge embeddings for the specified edges.
    """
    @staticmethod
    def hadamart(input_file: str, edges: List[Tuple[int, int]]) -> pd.DataFrame:
        """ Edge embeding with the hadamard distance.
        """
        print("Reading")
        node_embeddings = pd.read_csv(input_file)
        print("Dropping Nodes header")
        node_embeddings = node_embeddings.drop(["Nodes"], axis=1)
        print("to numpy")
        node_embeddings = node_embeddings.to_numpy()

        edge_embeddings = list()

        try:
            for i, j in edges:
                edge_embeddings.append(np.multiply(node_embeddings[i], node_embeddings[j]))
        except Exception as e:
            print("Error while concatenating")
            print(e)

        return node_embeddings, edge_embeddings

    @staticmethod
    def concatenate(input_file: str, edges: List[Tuple[int, int]]) -> pd.DataFrame:
        """ Edge embeding by concatenating node embeddings.
        """
        node_embeddings = pd.read_csv(input_file)
        node_embeddings = node_embeddings.drop(["Nodes"], axis=1)
        node_embeddings = node_embeddings.to_numpy()

        edge_embeddings = list()

        try:
            for i, j in edges:
                edge_embeddings.append(np.concatenate(([i, j], np.concatenatee((node_embeddings[i], node_embeddings[j])))))
        except Exception as e:
            print("Error while concatenating")
            print(e)

        return node_embeddings, edge_embeddings


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
                 node2vec, fairwalk")

    @staticmethod
    def dependencyError(algorithm, dependency):
        pass


# Valid recommendation policies.
policies = ["node2vec", "fairwalk"]

# Adjust input check according to the new features added.
if __name__ == "__main__":
    #############################
    # Parse command line inputs #
    #############################

    # Check valid number of command line arguments.
    if (len(sys.argv) != 9):
        InputErrors.argumentError()

    # Check obligatory arguments.
    if sys.argv[1] == "-i" and sys.argv[3] == "-e" and sys.argv[5] == "-p" and sys.argv[7] == "-o":
        input_file = sys.argv[2]
        edge_file = sys.argv[4]
        policy = sys.argv[6]
        output_file = sys.argv[8]
        edge_columns = ["Sources", "Targets"]
    else:
        InputErrors.argumentError()

    # Read edges.
    edges = pd.read_csv(edge_file, usecols=edge_columns, header=0, names=["Sources", "Targets"])
    edges = [(i, j) for i, j in edges.to_numpy()]

    # Get edge embeddings.
    if policy == "hadamart":
        nodeEmbeddings, edgeEmbeddings = EdgeEmbeding.hadamart(input_file, edges)
    elif policy == "concatenate":
        nodeEmbeddings, edgeEmbeddings = EdgeEmbeding.concatenate(input_file, edges)
    else:
        InputErrors.valueError()

    import os
    if os.path.exists(output_file+"_tmp"):
        os.remove(output_file+"_tmp")

    np.savetxt(output_file+"_tmp", edgeEmbeddings, delimiter=',')
    w = open(output_file, "w")
    w.write("Sources,Targets," + ",".join([str(i) for i in range(len(nodeEmbeddings[0]))]))
    i = 0
    with open(output_file+"_tmp", "r") as rT:
        for x in rT:
            w.write(f"{edges[i][0]},{edges[i][1]},{x}\n")
            i += 1
    w.close()
    os.remove(output_file+"_tmp")
