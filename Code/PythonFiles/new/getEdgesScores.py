""" Gets scores for candidate edges.

Gets the following scores for edges that doesn't exist in the graph and
that the target node is in maximum distance 3 from the source node. See
repository's README for exact definition of the scores below.

This script is not independable as it requires node2vec node
embeddings. We create a list of the files that it will need
to find inside the datasets folder and we raise the proper
messages / errors if the program isn't able to spot the on the folder
or copy them from the repository. In this case you should copy them
maunally.

Require files:
    1. 

Create file:
    1. edgeRecScores.txt: <sourceNode>\t<targetNode>\t<node2vecRecommendationScore>\t<resourceAllocationScore>\t
                        <jaccardCoefficientScore>\t<preferencialAttachmentScore>\t<addamicAddarScore>\n

Run inside datasets folder.         
"""
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
import numpy as np
import subprocess
import sys
import pickle
import networkx as nx
import time

# Useful classes and functions.
# TODO: Improve documentations of classes.
class Node:
    """ It represent a node object.

    Methods:
        addOutNeighbors(int):... 
        getOutNeighbors(set):...
    
    TODO: Improve class documentation.
    """
    def __init__(self):
        self.__outNeighbors = set()
        self.__notOutNeighbors = set()

    def addOutNeighbor(self, neighbor):
        self.__outNeighbors.add(neighbor)

    def getOutNeighbors(self):
        return self.__outNeighbors

    def getNotOutNeighbors(self):
         return self.__notOutNeighbors

    def setNotOutNeighbors(self, notOutNeighbors):
        self.__notOutNeighbors = notOutNeighbors

    def removeNotOutNeighbor(self, nodeId):
        self.__notOutNeighbors.discard(nodeId)

class Graph:
    def __init__(self):
        self.__nodeIds = set()
        self.__numberOfNodes = self.__loadNumberOfNodes()
        self.__nodes = [Node() for i in range(self.getNumberOfNodes())]
        self.__loadOutNeighbors()

    def __loadNumberOfNodes(self):
        """ Returns the number of the nodes in the graph.

        It reads the number of the nodes from the "out_graph.txt" file
        which should follow the conventions described in README file.
        """
        with open("out_graph.txt", "r") as file_one:
            return int(file_one.readline())

    def getNumberOfNodes(self):
        return self.__numberOfNodes

    def getNode(self, nodeId):
        return self.__nodes[nodeId]

    def __loadOutNeighbors(self):
        with open("out_graph.txt", "r") as fileOne:
            fileOne.readline()
            for line in fileOne:
                info = line.split()
                sourceNode = int(info[0])
                outNeighbor = int(info[1])
                self.addOutNeighbor(sourceNode, outNeighbor)
                self.__nodeIds.add(sourceNode)
                self.__nodeIds.add(outNeighbor)

    def addOutNeighbor(self, sourceNode, outNeighbor):
        self.getNode(sourceNode).addOutNeighbor(outNeighbor)
        self.getNode(sourceNode).removeNotOutNeighbor(outNeighbor)

    def getNodeIds(self):
        return self.__nodeIds

    def setNotOutNeighbors(self, nodeId):
        outNeighbors = self.getNode(nodeId).getOutNeighbors()
        # Add self cuase we don't allow self links.
        outNeighbors.add(nodeId)
        notOutNeighbors = self.getNodeIds().difference(outNeighbors)
        self.getNode(nodeId).setNotOutNeighbors(notOutNeighbors)

    def getOutNeighbors(self, nodeId):
        return self.getNode(nodeId).getOutNeighbors()

    def getNotOutNeighbors(self, nodeId):
        return self.getNode(nodeId).getNotOutNeighbors()

class NodeEmbedding:
    """ This class contains methods for the various different 
    techniques of embeddings.

    All methods existing in this class are static. We just have a used
    a class to group the node embedding methods.
    """
    @staticmethod
    def node2vec():
        """ It runs the node2vec cpp executable.

        Link: https://github.com/snap-stanford/snap/tree/master/examples/node2vec

        Returns file with the node embeddings.

        TODO: Add to function Parameters of cpp executables and change
        the current hardcoded mode.
        """
        try:
            subprocess.call(['cp ../../../snap/examples/node2vec/node2vec .'], cwd=".", shell=True)
            subprocess.call(["./node2vec -i:out_graph.edgelist -o:out_nodeEmbeddings.txt -l:3 -d:128 -p:0.3 -dr -v"], cwd=".", shell=True) # Pending for arguments.
        except:
            raise Exception("./node2vec didn't run properly! Check that executable exists in the root folder" + 
                                " or that executable is reachable from the script\n")

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

class EdgeClassifier:
    """ Trains edge classifier - Classify edges. 

    It does that with the use of a binary classifier that returns
    a probability for a node to belong at a chosen category.
    """
    def __init__(self):
        self.__g = nx.DiGraph()
        self.__loadGraph()
        self.__positiveTrainSample = []
        self.__negativeTrainSample = []
        self.__positiveTestSample = []
        self.__negativeTestSample = []
        self.__positiveTrainEmbeddings = []
        self.__negativeTrainEmbeddings = []
        self.__positiveTestEmbeddings = []
        self.__negativeTestEmbeddings = []

    def __loadGraph(self):
        self.__g = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.DiGraph())
        communityFile = np.loadtxt("out_community.txt", dtype = int, skiprows=1)

        for info in communityFile:
            node = info[0]
            cat = info[1]
            self.__g.nodes[node]['Category'] = cat

    def getPositiveSample(self):
        """ Gets an edges positive sample.
        
        It takes as sample the 20% of the edges
        so that the graph without them is still connected.

        TODO:Improve it. It is ridiculous how you choose sample.
        """
        #
        edgesToRemove = self.__g.number_of_edges() // 5
        edges = list(self.__g.edges)
        removedEdges = []
        edgesRemoved = 0
        pendingEdges = []
        tries = 0
        outOfGameNodes = set()

        # Remove nodes with small degree.
        print("Checking nodes for degree!")
        for node in self.__g.nodes:
            if self.__g.degree(node) < 5:
                outOfGameNodes.add(node)

        # Remove edges with in + out degree == 1.
        print("remove edges with nodes out of the game\n")
        for edge in edges:
            if edge[0] in outOfGameNodes or edge[1] in outOfGameNodes:
                edges.remove(edge)

        while edgesRemoved < edgesToRemove:

            tries += 1
            index = random.randrange(len(edges))
            edge = edges[index]

            pendingEdges.append(edge)
            edges.remove(edge)
            self.__g.remove_edge(edge[0], edge[1])

            # Check for connectivity.
            if tries % 100 == 0:
                if nx.is_weakly_connected(self.__g):
                    print("Succesful Sampling\n")
                    removedEdges += pendingEdges
                    edgesRemoved = len(removedEdges)
                    pendingEdges = []
                else:
                    print("Wrong Sampling\n")
                    edges += pendingEdges
                    for pendEdge in pendingEdges:
                        self.__g.add_edge(pendEdge[0], pendEdge[1])
                    pendingEdges = []

            

            if tries == edgesToRemove*10:
                sys.exit("Not able to get Positive sample")
        
        # Write edgelist without test sample for node2vec.
        nx.write_edgelist(self.__g, "out_graph.edgelist", data=False)

        self.__positiveTrainSample = list(self.__g.edges)
        self.__positiveTestSample = removedEdges.copy()

        for edge in removedEdges:
            self.__g.add_edge(edge[0], edge[1])

        print("Number of positive train samples: %d" %len(removedEdges) )
        print("Number of positive test samples: %d" %(len(edges) ) )

    def getNegativeSample(self, allTrain=False):
        """ Gets an edges negative sample.
        
        It takes a sample of edges that don't exist in graph
        equal to 50% of the graph's edges.
        """
        #
        numberOfEdges = self.__g.number_of_edges()
        sampleLength = numberOfEdges
        numberOfNodes = self.__g.number_of_nodes()
        negativeSample = []

        i = 0
        # TODO: Check not taking again same edge.
        while i < sampleLength:
            x = np.random.randint(0, numberOfNodes)
            y = np.random.randint(0, numberOfNodes)
            edge = (x,y)

            if not self.__g.has_edge(x, y):
                negativeSample.append(edge)
                i += 1

        if allTrain:
            self.__negativeTrainSample = negativeSample.copy()
        else:
            self.__negativeTrainSample = negativeSample[len(negativeSample) // 5:]
            self.__negativeTestSample = negativeSample[0:len(negativeSample) // 5]
        
        print("Number of train negative samples: ", len(self.__negativeTrainSample))
        print("Number of test negative samples: ", len(self.__negativeTestSample))

    def getEdgeEmbeddings(self, aucComp = True):
        """ Compute edge embeddings.

        TODO: Make type of edge embedding option when
            object is constructed.
        """
        #self.__positiveTrainEmbeddings = EdgeEmbeding.concatNodeEmbedding(self.__positiveTrainSample)
        #self.__negativeTrainEmbeddings = EdgeEmbeding.concatNodeEmbedding(self.__negativeTrainSample)
        #self.__positiveTestEmbeddings = EdgeEmbeding.concatNodeEmbedding(self.__positiveTestSample)
        #self.__negativeTestEmbeddings = EdgeEmbeding.concatNodeEmbedding(self.__negativeTestSample)

        if aucComp:
            self.__positiveTrainEmbeddings = EdgeEmbeding.hadamart(self.__positiveTrainSample)
            self.__negativeTrainEmbeddings = EdgeEmbeding.hadamart(self.__negativeTrainSample)
            self.__positiveTestEmbeddings = EdgeEmbeding.hadamart(self.__positiveTestSample)
            self.__negativeTestEmbeddings = EdgeEmbeding.hadamart(self.__negativeTestSample)
        else:
            print("Taking positive edge embeding\n")
            self.__positiveTrainEmbeddings = EdgeEmbeding.hadamart(self.__positiveTrainSample)
            print("Taking negative edge embeding\n")
            self.__negativeTrainEmbeddings = EdgeEmbeding.hadamart(self.__negativeTrainSample)

    def trainLogisticRegression(self, aucComp = True):
        """ It trains a logistic regression model.
        
        It evaluates model with AUC. It saves the model.

        Creates:
            "edgeClassifier.sav": the trained model.
            "outAuc.txt": The AUC score.
        """
        print("Start Training")

        if aucComp:
            self.getEdgeEmbeddings()
            xTrain = np.concatenate( (self.__positiveTrainEmbeddings, self.__negativeTrainEmbeddings), axis=0)
            yTrain = np.array([1 for i in range(len(self.__positiveTrainEmbeddings) )] + [0 for i in range(len(self.__negativeTrainEmbeddings) )])
            xTest = np.concatenate( (self.__positiveTestEmbeddings, self.__negativeTestEmbeddings), axis=0)
            yTest = np.array([1 for i in range(len(self.__positiveTestEmbeddings) )] + [0 for i in range(len(self.__negativeTestEmbeddings) )])
        else:
            self.__positiveTrainSample = list(self.__g.edges)
            self.getNegativeSample(True)
            self.getEdgeEmbeddings(aucComp)
            xTrain = np.concatenate( (self.__positiveTrainEmbeddings, self.__negativeTrainEmbeddings), axis=0)
            yTrain = np.array([1 for i in range(len(self.__positiveTrainEmbeddings) )] + [0 for i in range(len(self.__negativeTrainEmbeddings) )])

        # fit the model.
        self.clf = LogisticRegression()
        self.clf.fit(xTrain, yTrain)

        # Save model.
        clfFile = 'edgeClassifier.sav'
        pickle.dump(self.clf, open(clfFile, 'wb'))

        ## Calculate and save AUC.
        if aucComp:
            # Evaluate model.
            ## Predict probabilities.
            probs = self.clf.predict_proba(xTest)
            # Probabilities belonging to category 1.
            probs = probs[0:,1]

            auc = roc_auc_score(yTest, probs)
            print("AUC: ", auc)

            with open("outAuc.txt", "w") as fileOne:
                fileOne.write("auc: " + str(auc))

        print("end training")

    def loadModel(self, fileName):
        """ Loads Edge classifying model from pickle file.
        """
        self.clf = pickle.load(open(fileName, 'rb'))

    def getEdgesScore(self):
        """ Gets the score for the candidate edges.

        Returns 

        TODO: Break into smaller methods. Refactor!!!
        """
        # Source Nodes
        sourceNodes = []
        with open('sourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.append(int(line) )
        edges = []
        fairnessScores = []
        
        for sourceNode in sourceNodes:
            with open('out_%dEdgesScores.txt' %sourceNode, "r") as fileOne:
                fileOne.readline()
                for line in fileOne:
                    info = line.split()
                    targetNode = int(info[0])
                    fairnessScore = float(info[1])
                    edges.append((sourceNode, targetNode) )
                    fairnessScores.append(fairnessScore)


        #
        numberOfEdges = len(edges)
        #edgeEmbedings = EdgeEmbeding.concatNodeEmbedding(edges)
        edgeEmbedings = EdgeEmbeding.hadamart(edges)
        probs = self.clf.predict_proba(edgeEmbedings)
        probs = probs[0:,1]

        with open("edgesScores.txt", "w") as fileOne:
            fileOne.write("EdgeSource\tEdgeTarget\tfairnessScore\trecommendationScore\n")
            for i in range(numberOfEdges):
                x = edges[i][0]
                y = edges[i][1]
                fileOne.write("%d\t%d\t%f\t%f\n" %(x, y,  fairnessScores[i], probs[i]))
    
    def loadEdge(self, fileName):
        edges = []
        with open(fileName, "r") as fileOne:
            for line in fileOne:
                edge = line.split()
                x = edge[0]
                y = edge[1]
                edge = (x, y)
                edges.append(edge)

        print("Number of edges: ", len(edges))

    def writeEdgeList(self):
        # Write edgelist without test sample for node2vec.
        nx.write_edgelist(self.__g, "out_graph.edgelist", data=False)

class Recommenders:
    """ Pending for grouping recommenders.
    """
    def __init__(self):
        self.__g = nx.Graph()
        self.__loadGraph()

    def __loadGraph(self):
        self.__g = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.Graph())
        communityFile = np.loadtxt("out_community.txt", dtype = int, skiprows=1)

        for info in communityFile:
            node = info[0]
            cat = info[1]
            self.__g.nodes[node]['Category'] = cat
    
    def getResourceAlocation(self):
        print("Compute Resource allocation scores...")
        # Gets source nodes.
        # Random Source Nodes.
        sourceNodes = set()
        with open('randomSourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.add(int(line) )
        # Best by pagerank Source Nodes.
        with open('pagerankBestSourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.add(int(line) )

        # Get recommendation score for all edges.
        for node in sourceNodes:
            edges = []
            with open("%dcandidateEdges.txt" %node, "r") as fileOne:
                for line in fileOne:
                    info = line.split()
                    edge = (int(info[0]), int(info[1]))
                    edges.append(edge)
            preds = nx.resource_allocation_index(self.__g, edges)
            print("Writes recommendation scores for node %d\n" %node)
            with open("%dedgeResourceAlocationScores.txt" %node, "w") as fileOne:
                for u, v, p in preds:
                    fileOne.write("%d\t%d\t%f\n" %(u, v, p) )

    def getjaccardCoefficient(self):
        print("Compute Jaccard coefficient scores...")
        # Gets source nodes.
        # Random Source Nodes.
        sourceNodes = set()
        with open('randomSourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.add(int(line) )
        # Best by pagerank Source Nodes.
        with open('pagerankBestSourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.add(int(line) )

        # Get recommendation score for all edges.
        for node in sourceNodes:
            edges = []
            with open("%dcandidateEdges.txt" %node, "r") as fileOne:
                for line in fileOne:
                    info = line.split()
                    edge = (int(info[0]), int(info[1]))
                    edges.append(edge)
            
            preds = nx.jaccard_coefficient(self.__g, edges)
            print("Writes recommendation scores for node %d\n" %node)
            with open("%dedgeJaccardCoefficientScores.txt" %node, "w") as fileOne:
                for u, v, p in preds:
                    fileOne.write("%d\t%d\t%f\n" %(u, v, p) )

    def getAddamicAdar(self):
        print("Compute Addamic-Addar scores...")
        # Gets source nodes.
        # Random Source Nodes.
        sourceNodes = set()
        with open('randomSourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.add(int(line) )
        # Best by pagerank Source Nodes.
        with open('pagerankBestSourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.add(int(line) )

        # Get recommendation score for all edges.
        for node in sourceNodes:
            edges = []
            with open("%dcandidateEdges.txt" %node, "r") as fileOne:
                for line in fileOne:
                    info = line.split()
                    edge = (int(info[0]), int(info[1]))
                    edges.append(edge)
            
            preds = nx.adamic_adar_index(self.__g, edges)
            print("Writes recommendation scores for node %d\n" %node)
            with open("%dedgeAddamicAddarScores.txt" %node, "w") as fileOne:
                for u, v, p in preds:
                    fileOne.write("%d\t%d\t%f\n" %(u, v, p) )

    def getPreferencialAttachment(self):
        print("Compute Preferencial attachment scores...")
        # Gets source nodes.
        # Random Source Nodes.
        sourceNodes = set()
        with open('randomSourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.add(int(line) )
        # Best by pagerank Source Nodes.
        with open('pagerankBestSourceNodes.txt', 'r') as fileOne:
            fileOne.readline()
            for line in fileOne:
                sourceNodes.add(int(line) )

        # Get recommendation score for all edges.
        for node in sourceNodes:
            edges = []
            with open("%dcandidateEdges.txt" %node, "r") as fileOne:
                for line in fileOne:
                    info = line.split()
                    edge = (int(info[0]), int(info[1]))
                    edges.append(edge)
            
            preds = nx.preferential_attachment(self.__g, edges)
            print("Writes recommendation scores for node %d\n" %node)
            with open("%dedgePrefferentialAttachmentScores.txt" %node, "w") as fileOne:
                for u, v, p in preds:
                    fileOne.write("%d\t%d\t%d\n" %(u, v, p) )

def getCandidateEdges():
    """ Create file for each source node with the candidate edges.
    """
    print("Gets candidate edges...\n")
    # Init graph.
    graph = Graph()
    # Gets source nodes.
    # Random Source Nodes.
    sourceNodes = set()
    with open('randomSourceNodes.txt', 'r') as fileOne:
        fileOne.readline()
        for line in fileOne:
            sourceNodes.add(int(line) )
    # Best by pagerank Source Nodes.
    with open('pagerankBestSourceNodes.txt', 'r') as fileOne:
        fileOne.readline()
        for line in fileOne:
            sourceNodes.add(int(line) )
        
    for node in sourceNodes:
        graph.setNotOutNeighbors(node)
        notOutNeighbors = graph.getNode(node).getNotOutNeighbors()
        with open("%dcandidateEdges.txt" %node, "w") as fileOne:
            for nei in notOutNeighbors:
                fileOne.write("%d\t%d\n" %(node, nei))

def getEdgeRecommendationScores():
    print("Compute Recommendation scores...")
    # Load recommender.
    linkRecommender = pickle.load(open('edgeClassifier.sav', 'rb'))
    # Gets source nodes.
    # Random Source Nodes.
    sourceNodes = set()
    with open('randomSourceNodes.txt', 'r') as fileOne:
        fileOne.readline()
        for line in fileOne:
            sourceNodes.add(int(line) )
    # Best by pagerank Source Nodes.
    with open('pagerankBestSourceNodes.txt', 'r') as fileOne:
        fileOne.readline()
        for line in fileOne:
            sourceNodes.add(int(line) )

    # Get recommendation score for all edges.
    for node in sourceNodes:
        edges = []
        with open("%dcandidateEdges.txt" %node, "r") as fileOne:
            for line in fileOne:
                info = line.split()
                edge = (int(info[0]), int(info[1]))
                edges.append(edge)

        edgeEmbeddings = EdgeEmbeding.hadamart(edges)
        edgeRecommendationScore = linkRecommender.predict_proba(edgeEmbeddings)
        edgeRecommendationScore = edgeRecommendationScore[0:,1]
        print("Writes recommendation scores for node %d\n" %node)
        with open("%dedgeRecommendationScores.txt" %node, "w") as fileOne:
            for i in range(len(edges)):
                fileOne.write("%d\t%d\t%f\n" %(edges[i][0], edges[i][1], edgeRecommendationScore[i]) )

def trainModel(auc = True):
    startTime = time.time()
    print("Train link recommender...\n")
    print("Initialize Objects..\n")
    clf = EdgeClassifier()
    initializeTime = time.time()
    if auc:
        print("Gets Train and Test sets...\n")
        clf.getPositiveSample()
        poritiveSampleTime = time.time()
        clf.getNegativeSample()
        negativeSampleTime = time.time()
    else:
        clf.writeEdgeList()
    print("Compute node embeddings...\n")
    NodeEmbedding.node2vec()
    node2vecTime = time.time()
    print("Train the model...\n")
    clf.trainLogisticRegression(auc)
    trainingTime=time.time()

    with open("trainTiming.txt", "w") as fileOne:
        fileOne.write("InitializingTime: %f sec\n" %(initializeTime - startTime) )
        if auc:
            fileOne.write("PositiveSamplingTime: %f sec\n" %(poritiveSampleTime - initializeTime) )
            fileOne.write("NegativeSamplingTime: %f sec\n" %(negativeSampleTime - poritiveSampleTime) )
            fileOne.write("Node2vecTime: %f sec\n" %(node2vecTime - negativeSampleTime) )
        else:
            fileOne.write("Node2vecTime: %f sec\n" %(node2vecTime - initializeTime) )
        fileOne.write("TrainClassifierTime: %f sec\n" %(trainingTime - node2vecTime) )
        fileOne.write("TotalTime: %f sec\n" %(time.time() - startTime) )

def getListOfPreds(preds, numberOfPreds):
    predsList = np.zeros(numberOfPreds)

    i = 0
    for u, v, p in preds:
        predsList[i] = p
        i += 1

    return predsList

# --------------------------------- main ------------------------------------.

# See if there is trained node2vec based classifier - link recommender.
# If so, load id.
try:
    # Load recommender.
    linkRecommender = pickle.load(open('edgeClassifier.sav', 'rb') )
except:
    print("No trained classifier spotted. We will try to train one...\n" +
            "Be careful in this case no AUC will be computed and there will not be any evaluation of the model...\n")
    # If not try train. If node2vec doesn't exist, the relative class
    # with raise an error.
    trainModel(False)
    linkRecommender = pickle.load(open('edgeClassifier.sav', 'rb') )

# Here you have your node2vec link recommender as
# linkRecommender.

# Get Source Nodes.
randomNodes = np.loadtxt('randomSourceNodes.txt', skiprows= 1, dtype= int)
bestRedNodes = np.loadtxt('redBestSourceNodes.txt', skiprows= 1, dtype= int)
bestBlueNodes = np.loadtxt('blueBestSourceNodes.txt', skiprows= 1, dtype= int)

print("Initialize Objects...\n")
# Init graph.
graph = nx.read_edgelist('out_graph.txt', nodetype= int, create_using= nx.DiGraph() )
unGraph = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.Graph() ) # Load graph as undirected.
nodes = np.concatenate((randomNodes, bestRedNodes, bestBlueNodes) )

# Write files header. It also clears it in case is already written.
with open('edgeRecScores.txt', 'w') as fileOne:
    fileOne.write('sourceNode\ttargetNode\tnode2vecRecommendationScore\tresourceAllocationScore\t'+
                    'jaccardCoefficientScore\tpreferencialAttachmentScore\tadamicAddarScore\tgain\texpectedGain\n')

# For each source node get their neighbors of maximum distance 3 and
# compute scores. Don't check for random source existing in red or blue
# nodes because it's highly unexpected in large networks.
print("Get scores")
# for gain score.
print('Compute gain score\n')
subprocess.call(['./getEdgeFairnessScore.out'], cwd='.', shell=True)
num = 0
startTime = time.time()
for node in nodes:
    num += 1
    #print("\tnode %d %d\n" %(node, num ))
    # Estimate total time.
    if num == 10:
        stopTime = time.time()
        ellapsedTime = stopTime - startTime
        print("It will approximatelly need: %f seconds more\n" %(29 * ellapsedTime) )
    # Get as candidate neighbors all neighbors.
    candidateNeighbors = set([i for i in range(graph.number_of_nodes() )])

    # Remove self from candidate Neighbors.
    candidateNeighbors.remove(node)
    # Remove neighbors from candindate neighbors. 
    for nei in graph.neighbors(node):
        candidateNeighbors.discard(nei)

    if len(candidateNeighbors) != 0:
        # Get candidate edges in a list.
        candidateEdges = [(node, nei) for nei in candidateNeighbors]

        # Compute node2vec recommendation score for edges.
        edgeEmbeddings = EdgeEmbeding.hadamart(candidateEdges)
        edgeRecommendationScore = linkRecommender.predict_proba(edgeEmbeddings)
        edgeRecommendationScore = edgeRecommendationScore[0:,1]

        # Compute Other recommendation scores.
        resourceAllocationPreds = nx.resource_allocation_index(unGraph, candidateEdges)
        jaccardCoefficientPreds = nx.jaccard_coefficient(unGraph, candidateEdges)
        preferencialAttachmentPreds = nx.preferential_attachment(unGraph, candidateEdges)
        adamicAdarPreds = nx.adamic_adar_index(unGraph, candidateEdges)
        
        # Compute gain score.
        gainScore = np.loadtxt('%dedgeFairnessScores.txt' %node, skiprows=1, usecols=1)
        gainPreds = [gainScore[j] for i, j in candidateEdges]

        # convert results to list.
        numberOfEdges = len(candidateEdges)
        resourceAllocationValues = getListOfPreds(resourceAllocationPreds, numberOfEdges)
        jaccardCoefficientValues = getListOfPreds(jaccardCoefficientPreds, numberOfEdges)
        preferencialAttachmentValues = getListOfPreds(preferencialAttachmentPreds, numberOfEdges)
        adamicAdarValues = getListOfPreds(adamicAdarPreds , numberOfEdges)

        # Log these score to "edgeScores.txt" file.
        with open('edgeRecScores.txt', 'a') as fileOne:
            edge = 0
            for source, target in candidateEdges:
                fileOne.write('%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n' 
                            %(source, target, edgeRecommendationScore[edge], resourceAllocationValues[edge], 
                            jaccardCoefficientValues[edge], preferencialAttachmentValues[edge], adamicAdarValues[edge],
                            gainPreds[edge], gainPreds[edge] * edgeRecommendationScore[edge]
                            ) )

                edge += 1
                
for node in nodes:
    subprocess.call(['rm %dedgeFairnessScores.txt' %node], cwd='.', shell=True)