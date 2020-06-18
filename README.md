## Notes (In progress) 

### General Informations.

This repository has been created to extend FairLar repository in order to make the experiments for Fair Recommendation Research. In this stage Contains basic information about the algorithms provided and how to run them.

### Dependencies:<br/>
- Python 3.
- C++ 11.
- Optional Compiler that supports openmmp.

    **Note:** make file is for gcc with openmp.

All the experiments were made in linux Ubuntu. To compile cpp we used gcc compiler.

1. Greedy single source (independent executable):
    
    - Compile:
        >` g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o singleSourceGreedy.out graph.cpp pagerank.cpp edgeAddition.cpp singleSourceGreedy.cpp`

    - Execute: 
        >` ./singleSourceGreedy.out -s <source node id\> -n <numberOfEdges\>`

    **Note:** You will also find the algorithm as a static method in EdgeAddition class.

2. Fast greedy single source (independent executable):
    
    - Compile:
        >` g++ --std=c++11 -Wall -Wextra -O3 -fopenmp -o singleSourceFastGreedy.out graph.cpp pagerank.cpp edgeAddition.cpp singleSourceFastGreedy.cpp`

    - Execute: 
        >` ./singleSourceFastGreedy.out -s <source node id\> -n <numberOfEdges\>`

    **Note:** You will also find the algorithm as a static method in EdgeAddition class

TODO:
-----

- [ ] Write down a todo list.