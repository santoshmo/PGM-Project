__author__ = 'kevin'
#Thi algorithm givesa generality score to a file based on yls_algorithm. The higher the score, the lower the generality
from doc_generality import *

fileHandle = open('input.txt','r')
Input_text = fileHandle.read()

gscore = doc_generality(Input_text)

print "the generality score for this document is", gscore