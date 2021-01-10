""" Basic pipeline for the computation of the recommendation scores.
"""
from subprocess import run


# 1. Get source nodes.
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getSourceNodes.py", "."])
run(["python3", "getSourceNodes.py", "-p", "random", "-a", "10", "-o", "random_source_nodes.csv"])

# 2. Get candidate edges.
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getCandidateEdges.py", "."])
run(["python3", "getCandidateEdges.py", "-i", "random_source_nodes.csv", "-d", "5", "-o", "candidate_edges.csv"])

# 3. Get positive edge sample
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getPositiveEdgeSample.py", "."])
run(["cp", "python3", "getPositiveEdgeSample.py", "-g", "out_graph.txt", "-p", "100", "-o", "positive_edge_sample.csv"])

# 4. Get negative edges sample.
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getNegativeEdgeSample.py", "."])
run(["cp", "python3", "getNegativeEdgeSample.py", "-g", "out_graph.txt", "-p", "100", "-o", "negative_edge_sample.csv"])

# 5. Get node2vec node embeddings.
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getNodeEmbeddings.py", "."])
run(["python3", "getNodeEmbeddings.py", "-g", "out_graph.txt", "-p", "node2vec", "-o", "node2vec_node_embeddings.csv"])

# 6. Get fairwalk node embeddings.
run(["python3", "getNodeEmbeddings.py", "-g", "out_graph.txt", "-p", "fairwalk", "-o", "fairwalk_node_embeddings.csv"])

# 7. Get candidates edges node2vec embeddings.
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getEdgeEmbeddings.py", "."])
run(["python3", "getEdgeEmbeddings.py", "-i", "node2vec_node_embeddings.csv", "-e", "candidate_edges.csv", "-p", "hadamart", "-o", "candidate_edges_node2vec_embeddings.csv"])

# 8. Get candidate edges fairwalk embeddings.
run(["python3", "getEdgeEmbeddings.py", "-i", "fairwalk_node_embeddings.csv", "-e", "candidate_edges.csv", "-p", "hadamart", "-o", "candidate_edges_fairwalk_embeddings.csv"])

# 9. Get positive edge sample node2vec embeddings.
run(["python3", "getEdgeEmbeddings.py", "-i", "node2vec_node_embeddings.csv", "-e", "positive_edge_sample.csv", "-p", "hadamart", "-o", "positive_sample_edges_node2vec_embeddings.csv"])

# 10. Get positive edge sample fairwalk embeddings.
run(["python3", "getEdgeEmbeddings.py", "-i", "fairwalk_node_embeddings.csv", "-e", "positive_edge_sample.csv", "-p", "hadamart", "-o", "positive_sample_edges_fairwalk_embeddings.csv"])

# 10. Get positive edge sample node2vec embeddings.
run(["python3", "getEdgeEmbeddings.py", "-i", "node2vec_node_embeddings.csv", "-e", "negative_edge_sample.csv", "-p", "hadamart", "-o", "negative_sample_edges_node2vec_embeddings.csv"])

# 11. Get positive edge sample fairwalk embeddings.
run(["python3", "getEdgeEmbeddings.py", "-i", "fairwalk_node_embeddings.csv", "-e", "negative_edge_sample.csv", "-p", "hadamart", "-o", "negative_sample_edges_fairwalk_embeddings.csv"])

# 12. Get node2vec classifier.
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getClassifier.py", "."])
run(["python3", "getClassifier.py", "-p", "positive_sample_edges_node2vec_embeddings.csv", "-n", "negative_sample_edges_node2vec_embeddings.csv", "-o", "node2vec_recommender.sav"])

# 13. Get fairwalk classifier.
run(["python3", "getClassifier.py", "-p", "positive_sample_edges_fairwalk_embeddings.csv", "-n", "negative_sample_edges_fairwalk_embeddings.csv", "-o", "fairwalk_recommender.sav"])

# 14. Get red absorbing probabilities.
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getRedAbsorbingProbs.py", "."])
run(["python3", "getRedAbsorbingProbs.py", "-g", "out_graph.txt", "-o", "red_absorbing_probabilities.csv"])

# 15. Get Adamic Adar scores.
run(["cp", "/mnt/sdb1/tsiou/FaiRec/Code/PythonFiles/getRecommendationScores.py", "."])
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "adamic-adar", "-o", "adamic_adar_scores.csv"])

# 16. Get Jaccard coefficient scores.
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "jaccard-coefficient", "-o", "jaccard_coefficient_scores.csv"])

# 17. Get resource allocation scores.
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "resource-allocation", "-o", "resource_allocation_scores.csv"])

# 18. Get preferencial attachment scores.
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "preferential-attachment", "-o", "preferential_attachment_scores.csv"])

# 19. Get node2vec scores.
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "from-classifier", "-c", "node2vec_recommender.sav", "-o", "node2vec_scores.csv"])

# 20. Get fairwalk scores.
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "from-classifier", "-c", "fairwalk_recommender.sav", "-o", "fairwalk_scores.csv"])

# 21. Get fair scores.
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "fair", "-o", "fair_scores.csv"])

# 22. Get multiplicative hybrid node2vec scores.
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "multiplicative-hybrid", "-f", "fair_scores.csv", "-c", "node2vec_scores.csv", "-o", "hybrid_node2vec_scores.csv"])

# 23. Get multiplicative hybrid fairwalk scores.
run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "multiplicative-hybrid", "-f", "fair_scores.csv", "-c", "fairwalk_scores.csv", "-o", "hybrid_node2vec_scores.csv"])
