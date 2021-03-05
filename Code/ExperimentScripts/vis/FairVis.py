import networkx as nx
import optparse
import pandas as pd
import sys
from pathlib import Path

if __name__ == "__main__":
    #############################
    # Parse command line inputs #
    #############################
    parser = optparse.OptionParser()
    parser.add_option('-g')
    parser.add_option('-c')
    parser.add_option('-p')
    parser.add_option('-o')
    options, args = parser.parse_args()

    # Check obligatory arguments
    if not options.g or not options.c or not options.p or not options.o:
        sys.exit("ERROR! No valid command line arguments\n\
                    use:\n\
                        python3 FairVis.py -g <graph_file> -c <community_file> -p <policy file> -o <output_file>\n\
                        e.g.\n\
                        >>> python3 FairVis.py -g out_graph.csv -c com.csv -p sc_fair.csv -o test")

    output = options.o

    com_df = pd.read_csv(options.c, names=["Nodes", "Community"], sep='\\s+', skiprows=1)
    graph_df = pd.read_csv(options.g, names=["src", "tgt"], sep="\t", skiprows=1)

    nodes = list()
    nodes.sort()
    coms_mask = {0: "blue", 1: "red"}

    df = pd.read_csv(options.p)
    edges = set()
    end_points = set()
    for index, row in df.iterrows():
        if row['Rounds'] == -1: continue
        edges_list = list(eval(str(row["Edges"])))
        for e in edges_list:
            edges.add((e[0], e[1], 500))
            end_points.add(e[0])
            end_points.add(e[1])

    for index, row in graph_df.iterrows():
        src = row['src']
        tgt = row['tgt']
        if (src, tgt, 1) not in edges:
            edges.add((src, tgt, 1))
        else:
            print("Exists: " + str((src, tgt, 1)))

    edges_df = pd.DataFrame(edges, columns=['source', 'target', 'value'])
    edges_df.to_csv(f"edges-{output}.csv")

    for index, row in com_df.iterrows():
        n = row['Nodes']
        c = row['Community']
        if n in end_points:
            nodes.append((n, coms_mask[c], 5000))
        else:
            nodes.append((n, coms_mask[c], 10))

    print(len(nodes))
    print(len(end_points))
    nodes_df = pd.DataFrame(nodes, columns =['name', 'group', 'size'])
    nodes_df.to_csv(f"nodes-{output}.csv")

    from subprocess import run
    import subprocess
    process = subprocess.call(["Rscript", "PlotFair.R", "--args", f"nodes-{output}.csv", f"edges-{output}.csv", f"{options.p}", f"{output}"])
