#!/bin/bash
python3 experiment_one_fairness.py -r 10 -s node2vec_scores.csv -o sc_node2vec.csv
python3 experiment_one_fairness.py -r 10 -s adamic_adar_scores.csv  -o sc_adamic_adar.csv
python3 experiment_one_fairness.py -r 10 -s jaccard_coefficient_scores.csv -o sc_jaccard-coefficient.csv
python3 experiment_one_fairness.py -r 10 -s resource_allocation_scores.csv  -o sc_resource-allocation.csv
python3 experiment_one_fairness.py -r 10 -s preferential_attachment_scores.csv  -o sc_preferential-attachment.csv
python3 experiment_one_fairness.py -r 10 -s fairwalk_scores.csv -o sc_fairwalk.csv
python3 experiment_one_fairness.py -r 10 -s fair_scores.csv -o sc_fair.csv
python3 experiment_one_fairness.py -r 10 -s hybrid_node2vec_scores.csv -o sc_hybrid.csv
