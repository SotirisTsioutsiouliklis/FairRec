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
    def policy_run(
        graph_file: str,
        output_file: str,
        groups_file=None,
        d=128,
        l=3,
        r=10,
        p=1.0,
        q=1.0,
    ):
        if groups_file is not None:
            # Copy fairwalk executale from snap.
            run(
                ["cp", "/mnt/sdb1/tsiou/snap-fair/examples/node2vec/fairwalk", "."],
                cwd=".",
            )
            # Run fairwalk executable.
            run(
                [
                    "./fairwalk",
                    f"-i:{graph_file}",
                    f"-g:{groups_file}",
                    f"-o:{output_file}",
                    f"-l:{l}",
                    f"-d:{d}",
                    f"-p:{p}",
                    f"-q:{q}",
                    "-dr",
                    "-v",
                ]
            )
        else:
            # Copy node2vec executale from snap.
            run(["cp", "/mnt/sdb1/tsiou/snap/examples/node2vec/node2vec", "."], cwd=".")
            # Run node2vec executable.
            run(
                [
                    "./node2vec",
                    f"-i:{graph_file}",
                    f"-o:{output_file}",
                    f"-l:{l}",
                    f"-d:{d}",
                    f"-p:{p}",
                    f"-q:{q}",
                    "-dr",
                    "-v",
                ]
            )
        # Convert output to conventional output.
        with open(output_file, "r") as file_one:
            info = file_one.readline().split()
            numberOfNodes = int(info[0])
            embeddingDimension = int(info[1])
            nodeEmbeddings = np.array(
                [np.zeros(embeddingDimension) for i in range(numberOfNodes)]
            )

            for line in file_one:
                info = line.split()
                nodeId = int(info[0])

                for i in range(1, len(info)):
                    nodeEmbeddings[nodeId][i - 1] = float(info[i])

        # nodeEmbeddins are on id based order
        nodeEmbeddings = pd.DataFrame(nodeEmbeddings)
        nodeEmbeddings.insert(
            loc=0, column="Nodes", value=np.arange(0, numberOfNodes, 1, dtype=int)
        )
        nodeEmbeddings.to_csv(output_file, index=False)


# Adjust print message according to the new features added.
class InputErrors:
    @staticmethod
    def argumentError():
        """Terminates script due to input error and prints message
        with use instructions.
        """
        sys.exit(
            "ERROR! No valid command line arguments\n\
                use:\n\
                python3 getRecommendationScores.py -g <graph_file> -p <recommendation policy> -o <output_file>\n\
                e.g.\n\
                >>> python3 getRecommendationScores.py -g out_graph.txt -p node2vec -o adamic_adar_10.csv\n\n\
                    acceptable policies:\n\
                    node2vec, fairwalk"
        )

    @staticmethod
    def valueError():
        """Terminates script and print message for acceptable
        values.
        """
        sys.exit(
            "Error! Policy not valid.\n\
                 acceptable policies:\n\
                 node2vec, fairwalk"
        )

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
    import optparse

    parser = optparse.OptionParser()
    parser.add_option("-g")
    parser.add_option("-p")
    parser.add_option("--qp", default=1.0)
    parser.add_option("--pp", default=1.0)
    parser.add_option("--l", default=3)
    parser.add_option("--r", default=10)
    parser.add_option("--d", default=128)
    parser.add_option("-o")
    options, args = parser.parse_args()

    # Check obligatory arguments
    if not options.g or not options.p or not options.o:
        InputErrors.argumentError()

    graph_file = options.g
    policy = options.p
    output_file = options.o

    # Get node embeddings
    if policy == "node2vec":
        NodeEmbeddings.policy_run(
            graph_file,
            output_file,
            None,
            options.d,
            options.l,
            options.r,
            options.pp,
            options.qp,
        )
    elif policy == "fairwalk":
        # if graph_file is test.edgelist the groups file should be test.groups
        with open("out_community.txt", "r") as fileOne, open(
            "groups.txt", "w"
        ) as fileTwo:
            fileOne.readline()
            for line in fileOne:
                fileTwo.write(line)
        groups_file = "groups.txt"
        NodeEmbeddings.policy_run(
            graph_file,
            output_file,
            groups_file,
            options.d,
            options.l,
            options.r,
            options.pp,
            options.qp,
        )
        run(["rm", "groups.txt"])
    else:
        InputErrors.valueError()
