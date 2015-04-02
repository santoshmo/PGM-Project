import sys
import os
# Google Crawler
from pygoogle import pygoogle
import requests
import json
# Mallet for Python
import gensim 
# Text Extraction
from alchemyapi import AlchemyAPI
alchemyapi = AlchemyAPI()

import time
import random 
# while (1): 
	#option = raw_input("1) Query for a specific word\n2) Generate core raw text data\n3) Exit\n")

	# if (option == 1):
	# 	print option
	# 	query = raw_input("What query would you like to search?\n")
	# 	print "Query: " + str(query)

	# 	# get urls for a given query
	# 	g = pygoogle(query)
	# 	g.pages = 1
	# 	results = g.get_urls()
	# 	if len(results) <= 5:
	# 		print "not enough results, or getting blocked"
	# 		sys.exit(1)	

	# 	# store cleaned HTML from search results in temp dict
	# 	linkText = {}
	# 	count = 0
	# 	for result in results: 
	# 		response = alchemyapi.text('url', result)
	# 		if response['status'] == 'OK':
	# 			print "done with " + str(count)
	# 			linkText[count] = response['text'].encode('ascii','ignore')
	# 			count = count+1
	# 		else: 
	# 			continue

	# 	# make folder for query
	# 	os.makedirs(query)

	# 	#write cleaned HTML to .txt files
	# 	for i in range(0, 5):
	# 		docName = str(query) + '/' + str(query) + str(i) + ".txt"
	# 		fo = open(docName, "wb")
	# 		fo.write(str(linkText[i]).encode('ascii', 'ignore'))

	# 	print "Done with " + str(query)

	# elif (option == 2): 
		# Read from .txt file of queries
queries = open('queries_to_go.txt','r')
#os.makedirs("data")

for query in queries: 

	# generate random time out value 
	# to avoid detection of API abuse

	print "Query: " + str(query)

	# get urls for a given query
	g = pygoogle(query)
	g.pages = 1
	results = g.get_urls()
	if len(results) <= 5:
		print "not enough results, or getting blocked"
		continue
	
	# store cleaned HTML from search results in temp dict
	linkText = {}
	count = 0
	for result in results: 
		response = alchemyapi.text('url', result)
		if response['status'] == 'OK':
			print "done with " + str(count)
			linkText[count] = response['text'].encode('ascii','ignore')
			count = count+1
		else: 
			continue

	# make folder for query
	os.makedirs("data/"+query)

	#write cleaned HTML to .txt files
	for i in range(0, 5):
		docName = 'data/' + str(query) + '/' + str(query) + str(i) + ".txt"
		fo = open(docName, "wb")
		fo.write(str(linkText[i]).encode('ascii', 'ignore'))

	time.sleep(random.randint(1,60))

	# elif (option == 3): 
	# 	exit(0)

	# else: 
	# 	print "sorry, incorrect inputs"