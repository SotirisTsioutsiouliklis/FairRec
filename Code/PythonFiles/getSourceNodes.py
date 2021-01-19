""" Selects source nodes for the exeperiments and
stores them in a file.

You can specify different policies to select source nodes and the output
file to store them. The different policies of selecting souce nodes are:
    1. random: Selects randomly x% of the total number of the nodes in
    the network. x is a parameter specified as command line argument.

Examples:
    The general syntax is the following:

    >>> python3 getSourceNodes.py -p <policy> -a <additional_arguments> [-o <output_file>]

    Additional arguments is specified in the following policies to
    select source nodes:
        1. random:
            >>> python3 getSourceNodes.py -p random -a <percentage_of_nodes> [-o <output_file>]

            >>> python3 getSourceNodes.py -p random -a 10 -o random_source_nodes.csv

Notes:
    1. Additional arguments should be given in the specified order,
    as in the examples.
"""
import sys
import numpy as np
import pandas as pd


# Adjust print message according to the new features added.
class InputErrors:
    @staticmethod
    def argumentError():
        """ Terminates script due to input error and prints message
        with use instructions.
        """
        sys.exit("ERROR! No valid command line arguments\n\
                use:\n\
                >>> python3 getSourceNodes.py -p random -a <percentage_of_nodes> -of <output_file>\n\
                e.g. random\n\
                >>> python3 getSourceNodes.py -p random -a 10 -of random_source_nodes.csv")

    @staticmethod
    def percentage_error():
        """ Terminates script and print message for acceptable
        <percentage_of_nodes> values
        """
        sys.exit("Error! No valid argument for percentage in random selection.\n\
                 when\n\
                 \t>>> python3 getSourceNodes.py -p random -a <percentage_of_nodes> -of <output_file>\n\
                 then\n\
                 0 < <percentage_of_nodes> < 100")


def getRandomSourceNodes(percentage: int, output_file: str = None):
    """ Selects randomly percentage % source nodes of the network.
    """
    # Read number of nodes.
    number_of_nodes = 0
    with open("out_graph.txt", "r") as file_one:
        number_of_nodes = int(file_one.readline())
    # Creates array with the node ids.
    ids = np.arange(number_of_nodes, dtype=np.uint32)
    # Shuffles ids.
    rng = np.random.default_rng()
    rng.shuffle(ids)
    # Keep 10% of them.
    number_of_source_nodes = int((number_of_nodes * percentage) // 100)
    ids = ids[:number_of_source_nodes]

    # Set the name of the output file.
    if output_file is None:
        output_file = f"random_source_nodes_{percentage}.csv"
    w = open(output_file, "w")
    w.write("Nodes\n")

    for i in ids:
        w.write(f"{i}\n")
    w.close()


# Adjust input check according to the new features added.
if __name__ == "__main__":
    #############################
    # Parse command line inputs #
    #############################

    # Check valid number of command line arguments.
    if (not len(sys.argv) == 5) and (not len(sys.argv) == 7):
        InputErrors.argumentError()

    # Check obligatory argumnets.
    if (not sys.argv[1] == "-p") or (not sys.argv[3] == "-a"):
        InputErrors.argumentError()

    # Random policy.
    if sys.argv[2] == "random":
        percentage = float(sys.argv[4])
        # Check for valid percentage input.
        if not (percentage > 0 and percentage < 100):
            InputErrors.percentage_error()
        else:
            if len(sys.argv) == 7:
                if sys.argv[5] == "-o":
                    output_file = sys.argv[6]
                    getRandomSourceNodes(percentage, output_file)
                else:
                    InputErrors.argumentError()
            else:
                getRandomSourceNodes(percentage)
    # Handles all other incorrect inputs.
    else:
        InputErrors.argumentError()
