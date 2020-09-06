# Very simple Makefile to compile everything

all:
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getPagerank.out graph.cpp pagerank.cpp getPagerank.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getSourceNodes.out graph.cpp pagerank.cpp getSourceNodes.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getEdgeFairnessScore.out graph.cpp pagerank.cpp getEdgeFairnessScore.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o experimentOneRandom.out graph.cpp pagerank.cpp experimentOneRandom.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o experimentOneRed.out graph.cpp pagerank.cpp experimentOneRed.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o experimentOneBlue.out graph.cpp pagerank.cpp experimentOneBlue.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o experimentOneGreedyRandom.out graph.cpp pagerank.cpp experimentOneGreedyRandom.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o experimentOneGreedyRed.out graph.cpp pagerank.cpp experimentOneGreedyRed.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o experimentOneGreedyBlue.out graph.cpp pagerank.cpp experimentOneGreedyBlue.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o absorbingProbs.out graph.cpp pagerank.cpp absorbingProbs.cpp

clean:
	#rm -f jump_optimization