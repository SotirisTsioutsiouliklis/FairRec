#!/bin/bash
python3 experiment_two_acceptance.py -r 10 -s node2vec_scores.csv -n node2vec_scores.csv -o accept_prob_node2vec_scores.csv
python3 experiment_two_acceptance.py -r 10 -s adamic_adar_scores.csv -n node2vec_scores.csv -o accept_prob_adamic_adar_scores.csv
python3 experiment_two_acceptance.py -r 10 -s jaccard_coefficient_scores.csv -n node2vec_scores.csv -o accept_prob_jaccard-coefficient.csv
python3 experiment_two_acceptance.py -r 10 -s resource_allocation_scores.csv -n node2vec_scores.csv -o accept_prob_resource-allocation.csv
python3 experiment_two_acceptance.py -r 10 -s preferential_attachment_scores.csv -n node2vec_scores.csv -o accept_prob_preferential-attachment.csv
python3 experiment_two_acceptance.py -r 10 -s fairwalk_scores.csv -n node2vec_scores.csv -o accept_prob_fairwalk.csv
python3 experiment_two_acceptance.py -r 10 -s fair_scores.csv -n node2vec_scores.csv -o accept_prob_fair.csv
