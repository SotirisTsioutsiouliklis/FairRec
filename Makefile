# Very simple Makefile to compile everything

all:
	#g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o red_personilize.out graph.cpp pagerank.cpp all_personilized_red.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o link_recommendation.out graph.cpp pagerank.cpp edge_add.cpp link_recommendation.cpp
clean:
	#rm -f jump_optimization