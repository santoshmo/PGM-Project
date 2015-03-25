__author__ = 'kevin'
#Thi algorithm givesa generality score to a file based on yls_algorithm. The higher the score, the lower the generality
from doc_generality import *
import glob
import os

def f_rank(score):
    sorted_score = sorted(score)
    size = len(score)
    rank = []
    for i in xrange(0,size):
        for j in xrange(0,size):
            if score[i] == sorted_score[j] and j+1 not in rank:
                rank.append(j+1)
    return rank

input_list = glob.glob('*.txt')
score = []
for fn in input_list:
    file = open(fn)
    file_text = file.read()
    if len(file_text) > 0:
        score.append(doc_generality(file_text))
    file.close()

print score

rank = f_rank(score)
print rank

output_name = 'rank_'+ input_list[0][0]+'.txt'
output_file = open(output_name,'w')
i = 0
for j in xrange(0,5):
        output_file.write(input_list[j]+": %s \n" % rank[i])
        i = i + 1