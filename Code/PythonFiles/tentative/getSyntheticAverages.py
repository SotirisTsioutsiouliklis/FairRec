"""
"""
import subprocess
import numpy as np
import pandas as pd 

sizes = [0.1, 0.3, 0.5]
homophilies = [0.1, 0.3, 0.5, 0.7, 0.9]
policies = ['Node2vec', 'ResourceAllocation', 'PreferencialAttachment', 'JaccardCoefficient', 'Gain', 'ExpGain', 
        'AdamicAdar'] #, 'Random'
kinds = ['Random', 'Red', 'Blue']

for size in sizes:
    for homophily in homophilies:
        print("------------------------- Homophily %.2f %.2f -------------------------------" % (size, homophily) )
        redRatio = dict()
        accProb = dict()
        for policy in policies:
            redRatio[policy] = dict()
            accProb[policy] = dict()
            for kind in kinds:
                redRatio[policy][kind] = np.zeros(11)
                accProb[policy][kind] = np.zeros(11)
                for network in range(10):
                    np.add(redRatio[policy][kind], np.loadtxt('homophily_%.2f_%.2f/network_%d/redRatioBy%s%sSources.txt' %(size, homophily, network, policy, kind) ), 
                        out= redRatio[policy][kind] )
                    np.add(accProb[policy][kind], np.loadtxt('homophily_%.2f_%.2f/network_%d/node2vecBy%s%sSources.txt' %(size, homophily, network, policy, kind) ), 
                        out= accProb[policy][kind] )
                redRatio[policy][kind] /= 10
                accProb[policy][kind] /= 10
                np.savetxt('homophily_%.2f_%.2f/redRatioBy%s%sSources.txt' %(size, homophily, policy, kind), redRatio[policy][kind])
                np.savetxt('homophily_%.2f_%.2f/node2vecBy%s%sSources.txt' %(size, homophily, policy, kind), accProb[policy][kind])
