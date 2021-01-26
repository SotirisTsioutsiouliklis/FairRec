# Very simple Makefile to compile everything

all:
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o compute_scores_per_round.out graph.cpp pagerank.cpp compute_scores_per_round.cpp
	#g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o temp.out graph.cpp pagerank.cpp temp.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o deletionScores.out graph.cpp pagerank.cpp experimentDel.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getFairScores.out graph.cpp pagerank.cpp getFairScores.cpp
	#g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o testFormula.out graph.cpp pagerank.cpp testFormula.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getPagerank.out graph.cpp pagerank.cpp getPagerank.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o personalized_testing.out graph.cpp pagerank.cpp personalized_testing.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getSourceNodes.out graph.cpp pagerank.cpp getSourceNodes.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getEdgeFairnessScore.out graph.cpp pagerank.cpp getEdgeFairnessScore.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o absorbingProbs.out graph.cpp pagerank.cpp absorbingProbs.cpp

clean:
	#rm -f jump_optimization