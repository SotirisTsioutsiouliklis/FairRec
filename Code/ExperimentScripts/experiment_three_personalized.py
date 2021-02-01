""" Measures the impact on fairness of the network.
Based on the specified algorithm

Examples:

    >>> python3 experiment_three_personalized.py -r <number of rounds> -s <scores file> -o <output file>

"""
import pandas as pd
import sys
import os
from subprocess import run


# Parse arguments.
if len(sys.argv) != 7:
    sys.exit("ERROR!")
if sys.argv[1] == "-r" and sys.argv[3] == "-s" and sys.argv[5] == "-o":
    rounds = int(sys.argv[2])
    scores_file = sys.argv[4]
    output_file = sys.argv[6]
else:
    sys.exit("ERROR!")

# Read Scores.
scores = pd.read_csv(scores_file)

# Split per source node.
scores_dict = dict()
for i in range(len(scores)):
    source = scores.iloc[i, 0]
    target = scores.iloc[i, 1]
    score = scores.iloc[i, 2]
    if source not in scores_dict:
        scores_dict[source] = list()
    scores_dict[source].append((target, score))

# Sort targets.
for key in scores_dict:
    scores_dict[key].sort(key=lambda x: x[1], reverse=True)

# Call cpp script.
run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/CppFiles/compute_personalized_scores_per_round.out", "."])
run(["./compute_personalized_scores_per_round.out"])
personalized_scores = pd.read_csv("temp_score.txt")
personalized_scores.to_csv(f"{output_file}_round_{-1}.csv", index=False)

temp_edges_file = 'temp_edges.txt'
if os.path.exists(temp_edges_file):
    os.remove(temp_edges_file)

for i in range(rounds):
    edges = list()
    for key in scores_dict:
        try:
            edges.append((key, scores_dict[key][i][0]))
        except:
            print(f"{key},\n {scores_dict[key]}")
    # Write edges.
    with open(temp_edges_file, "a") as edge_file:
        for k, l in edges:
            edge_file.write(f"{k},{l}\n")
    # Call cpp script.
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/CppFiles/compute_personalized_scores_per_round.out", "."])
    run(["./compute_personalized_scores_per_round.out"])
    personalized_scores = pd.read_csv("temp_score.txt")
    personalized_scores.to_csv(f"{output_file}_round_{i}.csv", index=False)
run(["rm", temp_edges_file])
