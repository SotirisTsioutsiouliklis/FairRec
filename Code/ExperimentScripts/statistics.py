import networkx as nx
import pandas as pd
import sys

red_pr = pd.read_csv("red_absorbing_probabilities.csv")
pr = pd.read_csv("out_pagerank.csv")
com = pd.read_csv("out_community.txt", sep=" ")
G = nx.read_edgelist("out_graph.txt", nodetype=int, create_using=nx.DiGraph)
scores_files = ["fair", "node2vec", "fairwalk", "hybrid_node2vec"]

print(red_pr.info())
print(pr.info())
print(com.info())

for sf in scores_files:
    df = pd.read_csv(f"sc_{sf}.csv")
    trgs = dict()
    min = dict()
    sum = dict()
    max = dict()
    no_exist = dict()
    for index, row in df.iterrows():
        if row['Rounds'] == -1: continue
        edges = list(eval(str(row["Edges"])))
        for e in edges:
            trgs.setdefault(e[1], 0)
            min.setdefault(e[1], sys.maxsize)
            sum.setdefault(e[1], 0)
            max.setdefault(e[1], 0)
            no_exist.setdefault(e[1], 0)
            trgs[e[1]] += 1
            try:
                length = len(nx.shortest_path(G, source=e[1], target=e[0])) - 1
                sum[e[1]] += length
                if min[e[1]] > length: min[e[1]] = length
                if max[e[1]] < length: max[e[1]] = length
            except nx.NetworkXNoPath:
                no_exist[e[1]] += 1

    data = list()
    for k,v in trgs.items():
        data.append([k, None, v, G.in_degree(k), G.out_degree[k], 'red' if com[com["Nodes"] == k].iloc[0]["Community"] == 1 else 'blue', red_pr[red_pr["Nodes"] == k].iloc[0]["Red Pagerank"], pr[pr["Nodes"] == k].iloc[0]["Pagerank"], min[k], max[k], round(sum[k]/trgs[k],3), no_exist[k]])
    df = pd.DataFrame(data, columns=['id', 'name', 'appearances', 'in_degree', 'out_degree', 'community', 'red_pr', 'pr', 'min_path_length', 'max_path_length', 'average_path_length', 'no_paths'])
    df = df.sort_values(by=['appearances'])
    df.to_csv(f"{sf}_stats.csv", index=False)
