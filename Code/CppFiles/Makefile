# Very simple Makefile to compile everything

all:
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o singleSourceGreedy.out graph.cpp pagerank.cpp edgeAddition.cpp singleSourceGreedy.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o singleSourceFastGreedy.out graph.cpp pagerank.cpp edgeAddition.cpp singleSourceFastGreedy.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o multySourceGreedy.out graph.cpp pagerank.cpp edgeAddition.cpp multySourceGreedy.cpp	
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o multySourceFastGreedy.out graph.cpp pagerank.cpp edgeAddition.cpp multySourceFastGreedy.cpp	
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getSourceNodes.out graph.cpp pagerank.cpp getSourceNodes.cpp	
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o absorbingProbs.out graph.cpp pagerank.cpp edgeAddition.cpp absorbingProbs.cpp
	#g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o node_info.out graph.cpp pagerank.cpp edge_add.cpp node_info.cpp
	#g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o graph_info.out graph.cpp pagerank.cpp edge_add.cpp graph_info.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o getEdgeFairnessScore.out graph.cpp pagerank.cpp edgeAddition.cpp getEdgeFairnessScore.cpp
	g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o recommendationVsFairness.out graph.cpp pagerank.cpp edgeAddition.cpp recommendationVsFairness.cpp
clean:
	#rm -f jump_optimization