# PGM-Project
PGM Project

Contact "santoshm@umich.edu" with any problems. 

#Crawler
Requires AlchemyAPI and PyGoogle. 
To install AlchemyAPI, use the following. Note, "API_KEY" should be replaced with "api_key.txt" from the Crawler folder. 

	git clone https://github.com/AlchemyAPI/alchemyapi_python.git
	cd alchemyapi_python
	python alchemyapi.py YOUR_API_KEY
	python example.py

To install pygoogle (the relevant files are provided in Crawler/), simply run: 

	python setup.py install

Run 

	./genData.py 
	
Enter your search term. The resulting output should be the top N number of links from Google, named "Query0.txt", "Query1.txt",...,"QueryN.txt". These contained cleaned HTML, without tags, ads, or other nonsensical data. 

#Algorithms
Use algorithms in Algorithms to run tests and benchmark various queries
