import os
import sys
import numpy as np
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
import pickle
import networkx as nx
import random
from subprocess import run
import time


class EdgeEmbeding:
    """ It calculates the edge embeddings.

    There are various techniques to compute the edge embeddings.
    We use techniques based on node embeddings. All methods in this
    class are static. We use static methods to group the edge embedding
    methods.
    """
    @staticmethod
    def hadamart(edges):
        """ Edge embeding with the hadamard distance.
        
        Creates a file with the edge embeddings.

        Arguments:
            edges (list of tuples)
        Returns:
            embeddings (np.array): 2D array. Line i represents the
                    embedding for the tuple i.
        """
        #
        numberOfEdges = len(edges)

        # Load from embedding file the nodes' embeddings.
        with open("out_nodeEmbeddings.txt", "r") as file_one:
            info = file_one.readline().split()
            numberOfNodes = int(info[0])
            embeddingDimension = int(info[1])
            nodeEmbeddings = np.array([np.zeros(embeddingDimension) for i in range(numberOfNodes)])
            edgeEmbedings = np.array([np.zeros(embeddingDimension) for i in range(numberOfEdges)])
            for line in file_one:
                info = line.split()
                nodeId = int(info[0])

                for i in range(1, len(info)):
                    nodeEmbeddings[nodeId][i-1] = float(info[i])  

        # Calculate edge embeddings.
        for i in range(numberOfEdges):
            sourceNode = edges[i][0]
            targetNode = edges[i][1]

            edgeEmbedings[i] = np.multiply(nodeEmbeddings[sourceNode], nodeEmbeddings[targetNode]) 
        
        # Save edge embeddings.
        with open("out_edgeEmbeddings.txt", "w") as fileOne:
            fileOne.write("edge\tEmbedding\n")
            for i in range(numberOfEdges):
                fileOne.write("(%d,%d)" %(edges[i][0], edges[i][1]))
                for j in edgeEmbedings[i]:
                    fileOne.write("\t%f" %j)
                fileOne.write("\n")

        return edgeEmbedings

    @staticmethod
    def concatNodeEmbedding(edges):
        """ Edge embeding by concatenating node embeddings.
        
        Creates a file with the edge embeddings.

        Arguments:
            edges (list of tuples)
        Returns:
            embeddings (np.array): 2D array. Line i represents the
                    embedding for the tuple i.
        """
        #
        numberOfEdges = len(edges)

        # Load from embedding file the nodes' embeddings.
        with open("out_nodeEmbeddings.txt", "r") as file_one:
            info = file_one.readline().split()
            numberOfNodes = int(info[0])
            embeddingDimension = int(info[1])
            nodeEmbeddings = np.array([np.zeros(embeddingDimension) for i in range(numberOfNodes)])
            edgeEmbedings = np.array([np.zeros(2 * embeddingDimension) for i in range(numberOfEdges)])
            for line in file_one:
                info = line.split()
                nodeId = int(info[0])

                for i in range(1, len(info)):
                    nodeEmbeddings[nodeId][i-1] = float(info[i])  

        # Calculate edge embeddings.
        for i in range(numberOfEdges):
            sourceNode = edges[i][0]
            targetNode = edges[i][1]

            edgeEmbedings[i] = np.concatenate((nodeEmbeddings[sourceNode], nodeEmbeddings[targetNode]), axis=0 )

        # Save edge embeddings.
        with open("out_edgeEmbeddings.txt", "w") as fileOne:
            fileOne.write("edge\tEmbedding\n")
            for i in range(numberOfEdges):
                fileOne.write("(%d,%d)" %(edges[i][0], edges[i][1]))
                for j in edgeEmbedings[i]:
                    fileOne.write("\t%f" %j)
                fileOne.write("\n")

        return edgeEmbedings


# Get all subdirectories of current folder.
datasets = []
for root, dirs, files in os.walk("."):
    for d in dirs:
        datasets.append(os.path.join(root, d).strip('./'))
datasets.remove('ipynb_checkpoints')
datasets.remove('karate/.ipynb_checkpoints')

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

# Take recommendation scores by node sizes.
n_datasets = datasets[index_nodes]

with open("node2vec_elapsed_time.txt", "w") as file_one:
    file_one.write("dataset\ttime\n")

for dataset in n_datasets:
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/Python_files/get_node2vec_classifier.py", "."], cwd=f"{dataset}")
    start = time.time()
    run(['python3', 'get_node2vec_classifier.py'], cwd=f"{dataset}")
    elapsed = time.time() - start

    with open("node2vec_elapsed_time.txt", "a") as file_one:
        file_one.write(f"{dataset}\t{elapsed}\n")
