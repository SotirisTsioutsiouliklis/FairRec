## Notes (In progress) 

### General Informations.

This repository has been created to extend FairLar repository in order to make the experiments for Fair Recommendation Research. In this stage Contains basic information about the algorithms provided and how to run them.

### Dependencies:<br/>
- Python 3.
- C++ 11.
- Optional Compiler that supports openmmp.

    **Note:** make file is for gcc with openmp.

All the experiments were made in linux Ubuntu. To compile cpp we used gcc compiler.

### Repository Structure:<br/>
- FairLar
    - Code
        - Cpp_files
        - Python_files
    - experiments

### How to run the experiments:<br/> 

1. Experiment Zero: Compares  greedy with fast greedy algorithm for single source. It computes red pagerank ratio rise for both algorithms for 100 random source nodes and the 100 best by initial pagerank nodes. Then it takes the average red pagerank ratio for the two groups. For each source node it creates two files: "<node_id>_redPagerankGreedy.txt" and "<node_id>_redPagerankFastGreedy.txt".
    
    - Compile:
        >` g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o experimentZero.out graph.cpp pagerank.cpp experimentZero.cpp`

    - Execute: 
        >` ./experimentZero.out`

    **Note:** We then take the average red pagerank ratio for each group (random, best by pagerank).

1. Experiment Three: Compares the effect of adding edges based on recommendation score, on greatest red pagerank gain score (fast greedy algorithm) or on greatest expected red pagerank gain in average recommendation score, networks red pagerank ratio and expected red pagerank gain. It computes the forth mentioned scores for 100 random source nodes, for the 100 best by pagerank nodes and takes the average of them for each group (random, by pagerank).

    - Compile:
        >` g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o experimentThree.out graph.cpp pagerank.cpp experimentThree.cpp`

    - Execute: 
        >` ./experimentThree.out`

1. Fast greedy single source (independent executable):
    
    - Compile:
        >` g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o singleSourceFastGreedy.out graph.cpp pagerank.cpp edgeAddition.cpp singleSourceFastGreedy.cpp`

    - Execute: 
        >` ./singleSourceFastGreedy.out -s <source node id\> -n <numberOfEdges\>`

    **Note:** You will also find the algorithm as a static method in EdgeAddition class

TODO:
-----

- [ ] Write down a todo list.