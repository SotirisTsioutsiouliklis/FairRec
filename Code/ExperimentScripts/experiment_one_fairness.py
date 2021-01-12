""" Measures the impact on fairness of the network.
Based on the specified algorithm

Examples:

    >>> python3 experiment_one_fairness.py -r <number of rounds> -s <scores file> -o <output file>

"""
import pandas as pd
import sys
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


log = pd.DataFrame()
r = list()

for i in range(rounds):
    edges = list()
    for key in scores_dict:
        edges.append((key, scores_dict[key][i][0]))
    # Write edges.
    with open("temp_edges.txt", "a") as edge_file:
        for i, j in edges:
            edge_file.write(f"{i},{j}\n")
    # Call cpp script.
    run(["cp", "/mnt/sdb1/tsiou/FairRec/Code/CppFiles/compute_scores_per_round.out", "."])
    run(["./compute_scores_per_round.out"])
    with open("temp_score.txt", "r") as file_one:
        score = float(file_one.readline())
    r.append((i, str(edges), score))
run(["rm", "temp_edges.txt"])

# Save logs.
logs = pd.DataFrame(r, columns=["Rounds", "Edges", "Scores"])
logs.to_csv(output_file, index=False)
