# Very simple Makefile to compile everything

all:
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o pagerank.out graph.cpp pagerank.cpp all_personilized_red.cpp
clean:
	#rm -f jump_optimization