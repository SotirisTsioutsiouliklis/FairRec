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


######################
# Create train graph #
######################
# Read graph from edgelist.
digraph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.DiGraph())
# Sort id & degrees by degree.
degree = np.array([digraph.degree(i) for i in range(digraph.number_of_nodes())])
ids = np.arange(0, digraph.number_of_nodes(), 1)
index = np.argsort(-degree)
ids = ids[index]
degree = degree[index]
# Get nodes with more than 10 degree.
top_nodes = set([ids[i] for i in range(digraph.number_of_nodes()) if degree[i] >= 10])
# Compute wanted to remove edges.
wanted_edges = digraph.number_of_edges() // 8
removed_edges = set()
random_edges = list(digraph.edges)
random.shuffle(random_edges)
# Find edges to remove.
for i, j in random_edges:
    if i in top_nodes and j in top_nodes:
        removed_edges.add((i, j))

    if len(removed_edges) == wanted_edges:
        break
# Remove the edges from the initial graph.
for i, j in removed_edges:
    digraph.remove_edge(i, j)
# Store positive example.
with open("positive_sample.txt", "w") as file_one:
    for i, j in removed_edges:
        file_one.write(f"{i}\t{j}\n")
# Store train graph.
with open("out_graph.edgelist", "w") as file_one:
    for i, j in digraph.edges:
        file_one.write(f"{i}\t{j}\n")
# Get negative sample.
negative_sample = [(random.randint(0, digraph.number_of_nodes() - 1), random.randint(0, digraph.number_of_nodes() - 1))
                   for i in range(wanted_edges)]
# Store negative sample.
with open("negative_sample.txt", "w") as file_one:
    for i, j in negative_sample:
        file_one.write(f"{i}\t{j}\n")

################################
# Split to test and train sets #
################################
removed_edges = list(removed_edges)
positiveTrainSample = [i for i in removed_edges[0:digraph.number_of_edges() // 10]]
negativeTrainSample = [i for i in negative_sample[0:digraph.number_of_edges() // 10]]
positiveTestSample = [i for i in removed_edges[digraph.number_of_edges() // 10:]]
negativeTestSample = [i for i in negative_sample[digraph.number_of_edges() // 10:0]]

# Get node Embeddings from node2vec.
# Copy node2vec executable.
run(["cp", "~/Workspace/snap/examples/node2vec/node2vec", "."])
# Run node2vec with default settings.
run(["./node2vec", "-i:out_graph.edgelist", "-o:out_nodeEmbeddings.txt", "-l:3", "-d:128", "-p:0.3", "-dr", "-v"])

# Create edge embeddings.
# Train set.
positiveTrainEmbeddings = EdgeEmbeding.hadamart(positiveTrainSample)
negativeTrainEmbeddings = EdgeEmbeding.hadamart(negativeTrainSample)
# Test set.
positiveTestEmbeddings = EdgeEmbeding.hadamart(positiveTestSample)
negativeTestEmbeddings = EdgeEmbeding.hadamart(negativeTestSample)

# Train classifier.
# Get train/test sets.
xTrain = np.concatenate((positiveTrainEmbeddings, negativeTrainEmbeddings), axis=0)
yTrain = np.array([1 for i in range(len(positiveTrainEmbeddings))] + [0 for i in range(len(negativeTrainEmbeddings))])
xTest = np.concatenate((positiveTestEmbeddings, negativeTestEmbeddings), axis=0)
yTest = np.array([1 for i in range(len(positiveTestEmbeddings))] + [0 for i in range(len(negativeTestEmbeddings))])
# fit the model.
clf = LogisticRegression()
clf.fit(xTrain, yTrain)
# Save model.
pickle.dump(clf, open('edgeClassifier.sav', 'wb'))
# Evaluate model using AUC.
probs = clf.predict_proba(xTest)
# Probabilities belonging to category 1.
probs = probs[0:, 1]
auc = roc_auc_score(yTest, probs)
# Save AUC.
with open("outAuc.txt", "w") as fileOne:
    fileOne.write("auc: " + str(auc))
