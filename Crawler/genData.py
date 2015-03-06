#Time delay
import time
import random
# Google Crawler
from pygoogle import pygoogle
import requests
import json

# Text Extraction
from alchemyapi import AlchemyAPI
alchemyapi = AlchemyAPI()

# set up SQLite Database
import sqlite3
db = sqlite3.connect('cleanedHTML.db')
cur = db.cursor()
cur.execute('''DROP TABLE IF EXISTS Links''')

# Holds query, then cleaned HTML of first five search results as Blobs
cur.execute('''CREATE TABLE IF NOT EXISTS Links(
		Query varchar(100),
		LinkOne BLOB, 
		LinkTwo BLOB,
		LinkThree BLOB,
		LinkFour BLOB,
		LinkFive BLOB
	)''')

# Read from .txt file of queries
queries = open('queries.txt','r')
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
			linkText[count] = unicode(response['text'].encode('utf-8'), errors='replace')
			count = count+1
		else: 
			continue

	# push to DB
	cur.execute('''INSERT INTO Links(Query, LinkOne, LinkTwo, LinkThree, LinkFour, LinkFive) VALUES (?,?,?,?,?,?)''', (query, linkText[0], linkText[1], linkText[2], linkText[3], linkText[4]))
	db.commit()
	print "Done with " + str(query)
	time.sleep(random.randint(10,60))


db.close()


