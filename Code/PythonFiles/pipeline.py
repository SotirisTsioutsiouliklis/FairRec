""" Basic pipeline for the computation of the recommendation scores.
"""
from subprocess import run
import subprocess
from time import time

with open("log.txt", "w") as log_file:
    log_file.write("script,time\n")
    start = time()

    # 0. Get PageRank.
    print("Get PageRank")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/CppFiles/getPagerank.out", "."])
    cp = run(["./getPagerank.out"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getPagerank.out{time() - start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 1. Get source nodes.
    print("Get source nodes")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getSourceNodes.py", "."])
    cp = run(["python3", "getSourceNodes.py", "-p", "random", "-a", "10", "-o", "random_source_nodes.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getSourceNodes.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 2. Get candidate edges.
    print("Get candidate edges")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getCandidateEdges.py", "."])
    cp = run(["python3", "getCandidateEdges.py", "-i", "random_source_nodes.csv", "-d", "5", "-o", "candidate_edges.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getCandidateEdges.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 3. Get positive edge sample.
    print("Get positive edge sample")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getPositiveEdgeSample.py", "."])
    cp = run(["python3", "getPositiveEdgeSample.py", "-g", "out_graph.txt", "-p", "100", "-o", "positive_edge_sample.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getPositiveEdgeSample.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 4. Get negative edges sample.
    print("Get negative edge sample")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getNegativeEdgeSample.py", "."])
    cp = run(["python3", "getNegativeEdgeSample.py", "-g", "out_graph.txt", "-p", "100", "-o", "negative_edge_sample.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getNegativeEdgeSample.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 5. Get node2vec node embeddings.
    print("Get node2vec node embeddings")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getNodeEmbeddings.py", "."])
    cp = run(["python3", "getNodeEmbeddings.py", "-g", "out_graph.txt", "-p", "node2vec", "-o", "node2vec_node_embeddings.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getNodeEmbeddings.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 6. Get fairwalk node embeddings.
    print("Get fairwalk node embeddings")
    cp = run(["python3", "getNodeEmbeddings.py", "-g", "out_graph.txt", "-p", "fairwalk", "-o", "fairwalk_node_embeddings.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getNodeEmbeddings.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 7. Get candidates edges node2vec embeddings.
    print("Get node2vec edge embeddings")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getEdgeEmbeddings.py", "."])
    cp = run(["python3", "getEdgeEmbeddings.py", "-i", "node2vec_node_embeddings.csv", "-e", "candidate_edges.csv", "-p", "hadamart", "-o", "candidate_edges_node2vec_embeddings.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getEdgeEmbeddings.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 8. Get candidate edges fairwalk embeddings.
    print("Get fairwalk edge mbeddings")
    cp = run(["python3", "getEdgeEmbeddings.py", "-i", "fairwalk_node_embeddings.csv", "-e", "candidate_edges.csv", "-p", "hadamart", "-o", "candidate_edges_fairwalk_embeddings.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getEdgeEmbeddings.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 9. Get positive edge sample node2vec embeddings.
    print("Get positive edge sample node2vec embeddings")
    cp = run(["python3", "getEdgeEmbeddings.py", "-i", "node2vec_node_embeddings.csv", "-e", "positive_edge_sample.csv", "-p", "hadamart", "-o", "positive_sample_edges_node2vec_embeddings.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getEdgeEmbeddings.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 10. Get positive edge sample fairwalk embeddings.
    print("Get positive edge sample fairwalk embeddings")
    cp = run(["python3", "getEdgeEmbeddings.py", "-i", "fairwalk_node_embeddings.csv", "-e", "positive_edge_sample.csv", "-p", "hadamart", "-o", "positive_sample_edges_fairwalk_embeddings.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getEdgeEmbeddings.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 10. Get negative edge sample node2vec embeddings.
    print("Get negative edge sample node2vec embeddings")
    cp = run(["python3", "getEdgeEmbeddings.py", "-i", "node2vec_node_embeddings.csv", "-e", "negative_edge_sample.csv", "-p", "hadamart", "-o", "negative_sample_edges_node2vec_embeddings.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getEdgeEmbeddings.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 11. Get negative edge sample fairwalk embeddings.
    print("Get negative edge sample fairwalk embeddings")
    cp = run(["python3", "getEdgeEmbeddings.py", "-i", "fairwalk_node_embeddings.csv", "-e", "negative_edge_sample.csv", "-p", "hadamart", "-o", "negative_sample_edges_fairwalk_embeddings.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getEdgeEmbeddings.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 12. Get node2vec classifier.
    print("Get node2vec classifier")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getClassifier.py", "."])
    cp = run(["python3", "getClassifier.py", "-p", "positive_sample_edges_node2vec_embeddings.csv", "-n", "negative_sample_edges_node2vec_embeddings.csv", "-o", "node2vec_recommender.sav"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getClassifier.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 13. Get fairwalk classifier.
    print("Get fairwalk classifier")
    cp = run(["python3", "getClassifier.py", "-p", "positive_sample_edges_fairwalk_embeddings.csv", "-n", "negative_sample_edges_fairwalk_embeddings.csv", "-o", "fairwalk_recommender.sav"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getClassifier.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 14. Get red absorbing probabilities.
    print("Get red absorbing probabilites.")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getRedAbsorbingProbs.py", "."])
    cp = run(["python3", "getRedAbsorbingProbs.py", "-g", "out_graph.txt", "-o", "red_absorbing_probabilities.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRedAbsorbingProbs.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 15. Get Adamic Adar scores.
    print("Get adamic adar scores")
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/PythonFiles/getRecommendationScores.py", "."])
    cp = run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "adamic-adar", "-o", "adamic_adar_scores.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRecommendationScores.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 16. Get Jaccard coefficient scores.
    print("Get jaccard coefficient scores")
    cp = run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "jaccard-coefficient", "-o", "jaccard_coefficient_scores.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRecommendationScores.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 17. Get resource allocation scores.
    print("Get resource allocation scores")
    cp = run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "resource-allocation", "-o", "resource_allocation_scores.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRecommendationScores.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 18. Get preferencial attachment scores.
    print("Get prerferential attachment scores.")
    cp = run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "preferential-attachment", "-o", "preferential_attachment_scores.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRecommendationScores.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 19. Get node2vec scores.
    print("Get node2vec scores.")
    cp = run(["python3", "getRecommendationScores.py", "-i", "candidate_edges_node2vec_embeddings.csv", "-p", "from-classifier", "-c", "node2vec_recommender.sav", "-o", "node2vec_scores.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRecommendationScores.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 20. Get fairwalk scores.
    print("Get fairwalk scores.")
    cp = run(["python3", "getRecommendationScores.py", "-i", "candidate_edges_fairwalk_embeddings.csv", "-p", "from-classifier", "-c", "fairwalk_recommender.sav", "-o", "fairwalk_scores.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRecommendationScores.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 21. Get fair scores.
    print("Get fair scores.")
    cp = run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "fair", "-o", "fair_scores.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRecommendationScores.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()

    # 22. Get multiplicative hybrid node2vec scores.
    print("Get hybrid scores.")
    cp = run(["python3", "getRecommendationScores.py", "-i", "candidate_edges.csv", "-p", "multiplicative-hybrid", "-f", "fair_scores.csv", "-c", "node2vec_scores.csv", "-o", "hybrid_node2vec_scores.csv"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    log_file.write(f"getRecommendationScores.py{time() - temp_start}\n")
    log_file.write(f"{cp.stdout}\n")
    log_file.write(f"{cp.stderr}\n")
    log_file.write(f"{cp.returncode}\n")
    temp_start = time()
    log_file.write(f"Total: {time() - start}\n")
