""" Measures the average acceptance probability on the network.
Based on the specified algorithm.

Examples:

    >>> python3 experiment_two_acceptance.py -r <number of rounds> -s <scores file> -n <node2vec scores file> -o <output file>

"""
import pandas as pd
import sys


# Parse arguments.
if len(sys.argv) != 9:
    sys.exit("ERROR!")
if sys.argv[1] == "-r" and sys.argv[3] == "-s" and sys.argv[5] == "-n" and sys.argv[5] == "-v":
    rounds = int(sys.argv[2])
    scores_file = sys.argv[4]
    node2vec_file = sys.argv[6]
    output_file = sys.argv[8]
else:
    sys.exit("ERROR!")

# Read Scores.
scores = pd.read_csv(scores_file)
node2vec = pd.read_csv(node2vec_file, header=0, names=["Sources", "Targets", "Acceptance"])
df = scores.merge(node2vec, on=["Sources", "Targets"])

# Split per source node.
scores_dict = dict()
for i in range(len(df)):
    source = df["Sources"][i]
    target = df["Targets"][i]
    score = df["Scores"][i]
    acceptance = df["Acceptance"][i]
    if source not in scores_dict:
        scores_dict[source] = list()
    scores_dict[source].append((target, score, acceptance))

# Sort targets.
for key in scores_dict:
    scores_dict[key].sort(key=lambda x: x[1], reverse=True)

r = list()

acceptance = 0
for i in range(rounds):
    edges = list()
    for key in scores_dict:
        edges.append((key, scores_dict[key][0]))
        acceptance += scores_dict[key][2]
    r.append((i, str(edges), acceptance / (len(scores_dict) * i)))

# Save logs.
logs = pd.DataFrame(r, columns=["Rounds", "Edges", "Acceptance_prob"])
logs.to_csv(output_file, index=False)
