""" Creates a list of edges that doesn't belong to the graph.

We specify the graph file, the output file and the number of the sample
expressed as percentage of the number of the graph's edges.

Examples:

    >>> python3 getNegativeEdgeSample.py -g <graph file> -p <percentage> -o <output file>

    >>> python3 getNegativeEdgeSample.py -g out_graph.txt -p 100 -o edge_negative_sample.csv

Notes:
    1. Graph file is an edge list file which in the first line contains the number of the
    nodes in the graph.

TODO:
"""
import pandas as pd
import networkx as nx
import sys
from random import randint


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
                >>> python3 getNegativeEdgeSample.py -g <graph file> -p <percentage> -o <output file>\n\
                e.g.\n\
                >>> python3 getNegativeEdgeSample.py -g out_graph.txt -p 100 -o edge_negative_sample.csv"
        )

    @staticmethod
    def valueError():
        """Terminates script and print message for acceptable
        values.
        """
        sys.exit(
            "Error! Not valid percentage.\n\
                 0 < percentage <= 100"
        )

    @staticmethod
    def dependencyError(algorithm, dependency):
        pass


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
        percentage = float(sys.argv[4])
        output_file = sys.argv[6]
        if percentage <= 0 or percentage > 100:
            InputErrors.valueError()

    # Load graph.
    graph = nx.read_edgelist(graph_file, create_using=nx.DiGraph(), nodetype=int)
    number_of_nodes = graph.number_of_nodes()
    number_of_edges = graph.number_of_edges()
    wanted_number_of_edges = int(number_of_edges * (percentage / 100))
    edges = set([e for e in graph.edges])
    negative_edge_sample = set()

    # Generate negative sample.
    edge_order = 0
    w = open(output_file, "w")
    w.write("Sources,Targets\n")
    print("Generate Negative sample of edges. Progress:")
    while len(negative_edge_sample) < wanted_number_of_edges:
        e = (randint(0, number_of_nodes - 1), randint(0, number_of_nodes - 1))
        if e not in edges and e not in negative_edge_sample:
            negative_edge_sample.add(e)
            w.write(f"{e[0]},{e[1]}\n")
    w.close()
