""" Get node embeddings.

specify an embedding policy and get the node embeddings. This is a
"wrapper" script. The embeddings parameters are hard coded.

Examples:
    the general syntax is the following:

        >>> python3 getNodeEmbeddings.py -g <graph file> -p <embedding policy> -o <output file>

        >>> python3 getNodeEmbeddings.py -g out_graph.txt -p node2vec -o random_node2vec_node_embeddings.csv

Notes:
    1. Available policies:
        node2vec, fairwalk.

TODO:
    1. Add the ability to define embedding arguments.
    2. Add location to fairwalk executable.
    3. Add dependency errors.
"""
import sys
import numpy as np
import pandas as pd
from subprocess import run


class NodeEmbeddings:
    @staticmethod
    def node2vec(graph_file: str, output_file: str) -> pd.DataFrame:
        # Copy node2vec executale from snap.
        run(["cp", "/mnt/sdb1/tsiou/snap/examples/node2vec/node2vec", "."], cwd=".")
        # Run node2vec executable.
        run(["./node2vec", f"-i:{graph_file}", f"-o:{output_file}", "-l:3", "-d:128", "-p:0.3", "-dr", "-v"])
        # Convert output to conventional output.
        with open(output_file, "r") as file_one:
            info = file_one.readline().split()
            numberOfNodes = int(info[0])
            embeddingDimension = int(info[1])
            nodeEmbeddings = np.array([np.zeros(embeddingDimension) for i in range(numberOfNodes)])

            for line in file_one:
                info = line.split()
                nodeId = int(info[0])

                for i in range(1, len(info)):
                    nodeEmbeddings[nodeId][i - 1] = float(info[i])

        # nodeEmbeddins are on id based order
        nodeEmbeddings = pd.DataFrame(nodeEmbeddings)
        nodeEmbeddings.insert(loc=0, column="Nodes", value=np.arange(0, numberOfNodes, 1, dtype=int))

        return nodeEmbeddings

    @staticmethod
    def fairwalk(graph_file: str, output_file: str):
        sys.exit("fairwalk not implemented yet")
        # Copy fairwalk executale from snap.
        run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/CppFiles/getSourceNodes.out", "."], cwd=".")
        # Run fairwalk executable.
        run(["./node2vec", f"-i:{graph_file}", f"-o:{output_file}", "-l:3", "-d:128", "-p:0.3", "-dr", "-v"])


# Adjust print message according to the new features added.
class InputErrors:
    @staticmethod
    def argumentError():
        """ Terminates script due to input error and prints message
        with use instructions.
        """
        sys.exit("ERROR! No valid command line arguments\n\
                use:\n\
                python3 getRecommendationScores.py -g <graph_file> -p <recommendation policy> -o <output_file>\n\
                e.g.\n\
                >>> python3 getRecommendationScores.py -g out_graph.txt -p node2vec -o adamic_adar_10.csv\n\n\
                    acceptable policies:\n\
                    node2vec, fairwalk")

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
    if len(sys.argv) != 7:
        InputErrors.argumentError()

    # Check obligatory arguments.
    if sys.argv[1] == "-g" and sys.argv[3] == "-p" and sys.argv[5] == "-o":
        graph_file = sys.argv[2]
        policy = sys.argv[4]
        output_file = sys.argv[6]
    else:
        InputErrors.argumentError()

    # Get node embeddings.
    if policy == "node2vec":
        nodeEmbeddings = NodeEmbeddings.node2vec(graph_file, output_file)
    elif policy == "fairwalk":
        nodeEmbeddings = NodeEmbeddings.fairwalk(graph_file, output_file)
    else:
        InputErrors.valueError()

    nodeEmbeddings.to_csv(output_file, index=False)
