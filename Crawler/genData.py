import sys
# Google Crawler
from pygoogle import pygoogle
import requests
import json

# Text Extraction
from alchemyapi import AlchemyAPI
alchemyapi = AlchemyAPI()

# set up SQLite Database
# import sqlite3
# db = sqlite3.connect('cleanedHTML.db')
# cur = db.cursor()
# cur.execute('''DROP TABLE IF EXISTS Links''')

# Holds query, then cleaned HTML of first five search results as Blobs
# cur.execute('''CREATE TABLE IF NOT EXISTS Links(
# 		Query varchar(100),
# 		LinkOne BLOB, 
# 		LinkTwo BLOB,
# 		LinkThree BLOB,
# 		LinkFour BLOB,
# 		LinkFive BLOB
# 	)''')



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

#write cleaned HTML to .txt files
for i in range(0, 5):
	docName = str(query) + str(i) + ".txt"
	fo = open(docName, "wb")
	fo.write(str(linkText[i]).encode('ascii', 'ignore'))

# push to DB
#cur.execute('''INSERT INTO Links(Query, LinkOne, LinkTwo, LinkThree, LinkFour, LinkFive) VALUES (?,?,?,?,?,?)''', (query, linkText[0], linkText[1], linkText[2], linkText[3], linkText[4]))
#db.commit()
print "Done with " + str(query)


#db.close()


