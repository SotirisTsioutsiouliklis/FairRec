# Very simple Makefile to compile everything

all:
	#g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o personilized.out graph.cpp pagerank.cpp absoribing_probs.cpp
	#g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o link_recommendation.out graph.cpp pagerank.cpp edge_add.cpp link_recommendation.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o node_info.out graph.cpp pagerank.cpp edge_add.cpp node_info.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o graph_info.out graph.cpp pagerank.cpp edge_add.cpp graph_info.cpp
clean:
	#rm -f jump_optimization