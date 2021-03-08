import networkx as nx
import optparse
import pandas as pd
import sys
from pathlib import Path

def to_gexf(filename=None, nodes=None, edges=None, colors=None, layout=False):
     """
     Adding export option to Gephis GEXF format including timeline. Will export all instances and instance references including 'ctime' as creation time. The times can be overwritten by additional properties for 'ctime' (creation time) and 'dtime' (delete time) on instances and ireferences. A 'weight' property can be defined on both that will be used to set the node size and relationship weight.

     :param filename: GEXF File to write to
     :param nodes: Nodes to export
     :param edges: Edges to export
     :param colors: Map of concepts and ireftypes to colors as {'r': 255, 'g': 255, 'b': 255, 'a': 0}
     :param layout: True to precompute a Force Atlas layout
     :return:
     :limit: NetworkX graph
     """
     G = nx.DiGraph([],
         mode="dynamic"
     )
     if filename==None:
         filename = "out.gexf"

     # build color map
     if colors==None:
         import plotly
         ns = set([n[1] for n in nodes]) # get group
         colors={}
         cols=[c.replace('rgb(','').replace(')','').split(',') for c in plotly.colors.colorbrewer.Set3]
         cols=[{'r': int(c[0]), 'g': int(c[1]), 'b': int(c[2]), 'a': 0} for c in cols]
         for i, c in enumerate(ns):
             colors[c]=cols[i % len(cols)]
         cols=[c.replace('rgb(','').replace(')','').split(',') for c in plotly.colors.colorbrewer.Paired]
         cols=[{'r': int(c[0]), 'g': int(c[1]), 'b': int(c[2]), 'a': 0} for c in cols]
         for i, c in enumerate([0, 1]): # 0: existing edge , 1: recommended edge
             colors[c] = cols[i % len(cols)]
         print("Colors:" + str(colors))

     # build vertexes
     for n in nodes:
         G.add_node(
             n[0],
             viz={'color': colors[n[1]], 'size':n[2]}
         )

     # build edges
     for e in edges:
         G.add_edge(
             e[0],
             e[1],
             #label=ref['a'],
             #viz={'color': colors[e[2]]}
             #weight=refp.get('weight',1),
         )

     if layout:
         from fa2 import ForceAtlas2
         #pos=nx.spring_layout(G, seed=7, dim=1000, iterations=20, threshold=0.1)  # positions for all nodes - seed for reproducibility
         pos=ForceAtlas2().forceatlas2_networkx_layout(G, iterations=1000)
         for k,v in pos.items():
             G.nodes[k]['viz']['position']={'x':v[0], 'y':v[1], 'z':0.0}
     nx.write_gexf(G, filename)
     print("Check: " + filename)

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
    if not options.g or not options.c or not options.p:
        sys.exit("ERROR! No valid command line arguments\n\
                    use:\n\
                        python3 FairVis.py -g <graph_file> -c <community_file> -p <policy file> -o <output_file>\n\
                        e.g.\n\
                        >>> python3 FairVis.py -g out_graph.csv -c com.csv -p sc_fair.csv -o test")

    if not options.o:
        output = options.p.split(".")[0]
    else:
        output = options.o

    com_df = pd.read_csv(options.c, names=["Nodes", "Community"], sep='\\s+', skiprows=1)
    graph_df = pd.read_csv(options.g, names=["src", "tgt"], sep="\t", skiprows=1)

    nodes = list()
    nodes.sort()
    coms_mask_endpoints = {0: "blue", 1: "red", -1: "grey"}
    coms_mask = {0: "blue1", 1: "red1", -1: "grey"}
    df = pd.read_csv(options.p)
    edges = set()
    end_points = set()
    tgts = dict()
    for index, row in df.iterrows():
        if row['Rounds'] == -1: continue
        edges_list = list(eval(str(row["Edges"])))
        for e in edges_list:
            edges.add((e[0], e[1], 10))
            end_points.add(e[0])
            end_points.add(e[1])
            tgts.setdefault(e[0], 1)
            tgts.setdefault(e[1], 1)
            tgts[e[1]] += 1

    for index, row in graph_df.iterrows():
        src = row['src']
        tgt = row['tgt']
        if (src, tgt, 1) not in edges:
            edges.add((src, tgt, 1))

    # edges_df = pd.DataFrame(edges, columns=['source', 'target', 'value'])
    # edges_df.to_csv(f"edges-{output}.csv")

    for index, row in com_df.iterrows():
        n = row['Nodes']
        c = row['Community']
        if n in end_points:
            nodes.append((n, coms_mask_endpoints[c], 1 + (tgts[n]/len(edges_list))))
        else:
            nodes.append((n, coms_mask[c], 1))

    colors = { "blue": {'r': 0, 'g':0, 'b':255, 'a': 0}, "red": {'r': 255, 'g':40, 'b':0, 'a': 0}, 
               "blue1": {'r': 0, 'g':255, 'b':255, 'a': 0}, "red1": {'r': 241, 'g':179, 'b':179, 'a': 0},
               "grey": {'r': 58, 'g':59, 'b':60, 'a': 0}, 0: {'r': 166, 'g': 206, 'b': 227, 'a': 0}, 1: {'r': 31, 'g': 120, 'b': 180, 'a': 0}}
    to_gexf(filename=output +".gexf",nodes=nodes, edges=edges, colors=colors)
    # nodes_df = pd.DataFrame(nodes, columns =['name', 'group', 'size'])
    # nodes_df.to_csv(f"nodes-{output}.csv")

    # from subprocess import run
    # import subprocess
    # process = subprocess.call(["Rscript", "PlotFair.R", "--args", f"nodes-{output}.csv", f"edges-{output}.csv", f"{options.p}", f"{output}"])
