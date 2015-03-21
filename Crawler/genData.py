import sys
import os
# Google Crawler
from pygoogle import pygoogle
import requests
import json

# Text Extraction
from alchemyapi import AlchemyAPI
alchemyapi = AlchemyAPI()

query = raw_input("What query would you like to search?\n")
print "Query: " + str(query)

# get urls for a given query
g = pygoogle(query)
g.pages = 1
results = g.get_urls()
if len(results) <= 5:
	print "not enough results, or getting blocked"
	sys.exit(1)	

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
os.makedirs(query)

#write cleaned HTML to .txt files
for i in range(0, 5):
	docName = str(query) + '/' + str(query) + str(i) + ".txt"
	fo = open(docName, "wb")
	fo.write(str(linkText[i]).encode('ascii', 'ignore'))

print "Done with " + str(query)


