#!/bin/bash
python3 experiment_two_acceptance.py -r 10 -s node2vec_scores.csv -n node2vec_scores.csv -o accept_prob_node2vec_scores.csv
python3 experiment_two_acceptance.py -r 10 -s adamic_adar_scores.csv -n node2vec_scores.csv -o accept_prob_adamic_adar_scores.csv
python3 experiment_two_acceptance.py -r 10 -s jaccard-coefficient.csv -n node2vec_scores.csv -o accept_prob_jaccard-coefficient.csv
python3 experiment_two_acceptance.py -r 10 -s resource-allocation.csv -n node2vec_scores.csv -o accept_prob_resource-allocation.csv
python3 experiment_two_acceptance.py -r 10 -s preferential-attachment.csv -n node2vec_scores.csv -o accept_prob_preferential-attachment.csv
