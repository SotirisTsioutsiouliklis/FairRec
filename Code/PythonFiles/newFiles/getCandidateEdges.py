""" Get candidates edges for the specified source nodes.

You must specify the file from which the script will read the source
nodes using the pandas.read_csv(<file_name>,
usecols=[<source nodes column>]) and the distance for which it will
compute the candidate edges.

Examples:
    The general syntax is the following:

    >>> python3 getCandidateEdges.py -i <input_file> -c <column_name> -d <distance> -o <output_file>

    >>> python3 getCandidateEdges.py -i random_source_nodes_10.csv -c Nodes -d 3 -o candidate_edges_random_10.csv

Notes:


"""
import sys
import pandas as pd
import networkx as nx


# Adjust print message according to the new features added.
class InputErrors:
    @staticmethod
    def argumentError():
        """ Terminates script due to input error and prints message
        with use instructions.
        """
        sys.exit("ERROR! No valid command line arguments\n\
                use:\n\
                python3 getCandidateEdges.py -i <input_file> -c <column_name> -d <distance> -o <output_file>\n\
                e.g.\n\
                >>> python3 getCandidateEdges.py -i random_source_nodes_10.csv -c Nodes -d 3 -o candidate_edges_random_10.csv")

    @staticmethod
    def valueError():
        """ Terminates script and print message for acceptable
        <distance> values
        """
        sys.exit("Error! No valid argument for distance.\n\
                 when\n\
                 distance > 1")


def get_candidates(graph: nx.Graph, source: int, distance: int) -> set:
    neighbors = set()
    neighbors.add(source)
    # Find nodes in distance < than the distance.
    for i in range(1, distance):
        temp_neighbors = set()
        for j in neighbors:
            temp_neighbors = temp_neighbors.union(set([n for n in graph.neighbors(j)]))
        neighbors = neighbors.union(temp_neighbors)
    # Find candidates.
    candidate_edges = set()
    for j in neighbors:
        candidate_edges = candidate_edges.union(set([n for n in graph.neighbors(j) if n not in neighbors]))

    return candidate_edges


# Adjust input check according to the new features added.
if __name__ == "__main__":
    #############################
    # Parse command line inputs #
    #############################

    # Check valid number of command line arguments.
    if len(sys.argv) != 9:
        InputErrors.argumentError()

    # Check obligatory argumnets.
    if sys.argv[1] == "-i" and sys.argv[3] == "-c" and sys.argv[5] == "-d" and sys.argv[7] == "-o":
        input_file = sys.argv[2]
        column_name = sys.argv[4]
        distance = int(sys.argv[6])
        output_file = sys.argv[8]
        if not distance > 1:
            InputErrors.valueError()
    else:
        InputErrors.argumentError()

    # Candidate edges.
    candidate_edges = list()
    # Load source nodes.
    source_nodes = pd.read_csv(input_file)[column_name].to_numpy()
    # Load graph as undirected.
    unGraph = nx.read_edgelist("out_graph.txt", create_using=nx.Graph(), nodetype=int)
    # Find candidate neighbors for each source node.
    candidate_sources = list()
    candidate_targets = list()
    order = 0
    total_nodes = len(source_nodes)
    sys.stdout.write(f"{order / total_nodes}%")
    for source in source_nodes:
        candidate_neighbors = get_candidates(unGraph, source, distance)
        for target in candidate_neighbors:
            candidate_sources.append(source)
            candidate_targets.append(target)
        # Progress bar.
        order += 1
        sys.stdout.write("\r")
        sys.stdout.write(f"{(order / total_nodes) * 100}%")
    # Write candidates in a file.
    candidates = pd.DataFrame(data={"Sources": candidate_sources, "Targets": candidate_targets}, dtype=int)
    candidates.to_csv(output_file, index=False)
