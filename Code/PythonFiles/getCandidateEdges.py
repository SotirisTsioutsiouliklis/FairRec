""" Get candidates edges for the specified source nodes.

You must specify the file from which the script will read the source
nodes using the pandas.read_csv(<file_name>,
usecols=[<source nodes column>]) and the distance for which it will
compute the candidate edges.

Examples:
    The general syntax is the following:

    >>> python3 getCandidateEdges.py -i <input_file> -d <distance> -o <output_file>

    >>> python3 getCandidateEdges.py -i random_source_nodes_10.csv -d 3 -o candidate_edges_random_10.csv

Notes:
    1. input file should be readable with pd.read_csv() and the nodes exist in the columns "Nodes".

"""
import sys
import pandas as pd
import networkx as nx


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
                python3 getCandidateEdges.py -i <input_file> -c <column_name> -d <distance> -o <output_file>\n\
                e.g.\n\
                >>> python3 getCandidateEdges.py -i random_source_nodes_10.csv -c Nodes -d 3 -o candidate_edges_random_10.csv"
        )

    @staticmethod
    def valueError():
        """Terminates script and print message for acceptable
        <distance> values
        """
        sys.exit(
            "Error! No valid argument for distance.\n\
                 when\n\
                 distance > 1"
        )


def get_candidates(graph: nx.Graph, source: int, distance: int) -> set:
    neighbors = set()
    neighbors.add(source)
    # Find immediate neighbors.
    neighbors = neighbors.union(set([n for n in graph.neighbors(source)]))
    # Find nodes in distance < than the distance.
    candidate_edges = neighbors.copy()
    for i in range(2, distance + 1):
        for j in candidate_edges:
            candidate_edges = candidate_edges.union(
                set([n for n in graph.neighbors(j)])
            )
    candidate_edges = candidate_edges - neighbors

    # Comment out section computes neighbors of distance = , not <=.
    """
    # Find nodes in distance < than the distance.
    for i in range(1, distance):
        temp_neighbors = set()
        for j in neighbors:
            temp_neighbors = temp_neighbors.union(set([n for n in graph.neighbors(j)]))
        neighbors = neighbors.union(temp_neighbors)

    for j in neighbors:
        candidate_edges = candidate_edges.union(set([n for n in graph.neighbors(j) if n not in neighbors]))
    """

    return candidate_edges


# Adjust input check according to the new features added.
if __name__ == "__main__":
    #############################
    # Parse command line inputs #
    #############################

    # Check valid number of command line arguments.
    if len(sys.argv) != 7:
        InputErrors.argumentError()

    # Check obligatory argumnets.
    if sys.argv[1] == "-i" and sys.argv[3] == "-d" and sys.argv[5] == "-o":
        input_file = sys.argv[2]
        distance = int(sys.argv[4])
        output_file = sys.argv[6]
        if not distance > 1:
            InputErrors.valueError()
    else:
        InputErrors.argumentError()

    # Candidate edges.
    candidate_edges = list()
    # Load source nodes.
    source_nodes = pd.read_csv(input_file)["Nodes"].to_numpy()
    # Load graph as undirected.
    unGraph = nx.read_edgelist("out_graph.txt", create_using=nx.Graph(), nodetype=int)
    order = 0
    total_nodes = len(source_nodes)
    w = open(output_file, "w")
    w.write("Sources,Targes\n")
    for source in source_nodes:
        candidate_neighbors = get_candidates(unGraph, source, distance)
        for target in candidate_neighbors:
            w.write(f"{source},{target}\n")
