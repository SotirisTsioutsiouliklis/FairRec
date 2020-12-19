""" Script to evaluate the scalability of networkx
link prediction algorithms.
"""
import networkx as nx
import os
import sys
import numpy as np
import time
from collections import namedtuple
import sys

Times = namedtuple('Times', ['total', 'candidates', 'jaccard', 'preferential', 'adamic', 'resource'])

# Get all subdirectories of current folder.
datasets = []
for root, dirs, files in os.walk("."):
    for d in dirs:
        datasets.append(os.path.join(root, d).strip('./'))
datasets.remove('ipynb_checkpoints')

# Get number of nodes and edges for each dataset.
number_of_nodes = []
number_of_edges = []

for dataset in datasets:
    graph_file = open(f"{dataset}/out_graph.txt", "r")
    lines = graph_file.readlines()
    edges = len(lines) - 1
    nodes = int(lines[0])
    
    number_of_nodes.append(nodes)
    number_of_edges.append(edges)

# Convert to numpy arrays.
datasets = np.array(datasets)
number_of_nodes = np.array(number_of_nodes)
number_of_edges = np.array(number_of_edges)

# Get sort indexes by nodes and by edges.
index_nodes = np.argsort(number_of_nodes)
index_edges = np.argsort(number_of_edges)

# Get 10% source nodes for each datasets.
source_nodes = dict()
for i in range(len(datasets)):
    dataset = datasets[i]
    nodes_ids = np.arange(number_of_nodes[i])
    np.random.shuffle(nodes_ids)
    nodes_ids = nodes_ids[0: (number_of_nodes[i] // 10)]
    source_nodes[dataset] = nodes_ids

# Take recommendation scores by node sizes.
n_datasets = datasets[index_nodes]

# Take recommendation scores by node sizes.
n_datasets = datasets[index_nodes]

# Log times.
with open("elapsed_times.txt", "w") as file_one:
    file_one.write("dataset\ttotal\tcandidate\tjaccard\tadamic\tresource\tpreferntial\n")
n_dataset = 0
for dataset in n_datasets:
    # Progress prints.
    n_dataset += 1
    # Start timing.
    start = time.time()
    # Initialize graph.
    ungraph = nx.read_edgelist(f"{dataset}/out_graph.txt", nodetype=int, create_using=nx.Graph() )
    # Initialize candidate edges.
    candidate_edges = []
    # For each source node, take as candidate edges only nodes in distance 2.
    for source_node in source_nodes[dataset]:
        # Get neighbors.
        neighbors = [node for node in ungraph.neighbors(source_node)]
        neighbors_2 = []
        # Get neighbors of neighbors.
        for neighbor in neighbors:
            neighbors_2 += [node for node in ungraph.neighbors(neighbor)]
        # Keep non neighbors in candidate edges.
        candidate_edges += [(source_node, node) for node in neighbors_2 if node not in neighbors]
    # Candidates time.
    times_candidates = time.time() - start
    
    # Compute link prediction scores and log scores.
    preds = nx.jaccard_coefficient(ungraph, candidate_edges)
    with open(f"{dataset}/jaccard_scores.txt", "a") as file_one:
        file_one.write("source\ttarget\tscore\n")
        for u, v, p in preds:
            file_one.write(f"{u}\t{v}\t{p}\n")
    # Jaccard time.
    times_jaccard = time.time() - times_candidates
    
    preds = nx.resource_allocation_index(ungraph, candidate_edges)
    with open(f"{dataset}/allocation_scores.txt", "a") as file_one:
        file_one.write("source\ttarget\tscore\n")
        for u, v, p in preds:
            file_one.write(f"{u}\t{v}\t{p}\n")
    # Resource time.
    times_resource = time.time() - times_jaccard
    """
    preds = nx.adamic_adar_index(ungraph, candidate_edges)
    with open(f"{dataset}/adamic_scores.txt", "w") as file_one:
        file_one.write("source\ttarget\tscore\n") 
        for u, v, p in preds:
            file_one.write(f"{u}\t{v}\t{p}\n")
    # Adamic time.
    times_adamic = time.time() - times.resource
    """
    preds = nx.preferential_attachment(ungraph, candidate_edges)
    with open(f"{dataset}/preferencial_scores.txt", "a") as file_one:
        file_one.write("source\ttarget\tscore\n")
        for u, v, p in preds:
            file_one.write(f"{u}\t{v}\t{p}\n")
    # Preferential time.
    times_preferential = time.time() - times_resource
    
    # Total time.
    times_total = time.time() - start
    
    # Log time.
    with open("elapsed_times.txt", "a") as file_one:
        file_one.write(f"{dataset}\t{times_total}\t{times_candidates}\t{times_jaccard}\t{times.adamic}\t{times_resource}\t{times_preferential}\n")
        
    sys.stdout.write("\r")
    sys.stdout.write(f"dataset: {dataset} | progress: {(n_dataset / len(datasets)) * 100}%")
    sys.stdout.flush()    
            
        
