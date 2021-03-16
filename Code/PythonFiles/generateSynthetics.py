""" Generates synthetic networks using the synthetic_graph_generator.cpp
script of FairLar.
"""
from subprocess import run
import numpy as np


network_size = 5000
edges_per_node = 5
initial_nodes = 3
homophily_degrees = np.arange(0.1, 1.0, 0.1)
red_group_sizes = np.arange(0.1, 1.0, 0.1)

# Create symmetric datasets.
run(["mkdir", "symmetric"])
for size in red_group_sizes:
    for homophily in homophily_degrees:
        # Create folder.
        run(["mkdir", f"symmetric/size_{size:.2f}_homophily_{homophily:.2f}"])
        # Copy-Paste executable.
        run(["cp", "/mnt/sdb1/tsiou/FairLaR/Code/Cpp_files/homophily_graph_generator.out", f"symmetric/size_{size:.2f}_homophily_{homophily:.2f}"])
        # Create dataset.
        run(["./homophily_graph_generator.out", f"{homophily:.2f}", f"{homophily:.2f}", f"{size:.2f}",
             f"{network_size}", f"{edges_per_node}", f"{initial_nodes}"], cwd=f"symmetric/size_{size:.2f}_homophily_{homophily:.2f}")

# Create asymmetric datasets. Protected group neutral.
run(["mkdir", "asymmetric"])
for size in red_group_sizes:
    for homophily in homophily_degrees:
        # Create folder.
        run(["mkdir", f"asymmetric/size_{size:.2f}_homophily_{homophily:.2f}"])
        # Copy-Paste executable.
        run(["cp", "/mnt/sdb1/tsiou/FairLaR/Code/Cpp_files/homophily_graph_generator.out", f"asymmetric/size_{size:.2f}_homophily_{homophily:.2f}"])
        # Create dataset.
        run(["./homophily_graph_generator.out", f"{homophily:.2f}", "0.5", f"{size:.2f}",
             f"{network_size}", f"{edges_per_node}", f"{initial_nodes}"], cwd=f"asymmetric/size_{size:.2f}_homophily_{homophily:.2f}")
