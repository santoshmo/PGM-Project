from nltk.corpus import wordnet as wn
''' 
	Topic Model container -- list of words
'''
class TopicModel: 
	
	def __init__(self, words):
		self.words = []
		self.similarityToParent = 0
		for word in words: 
			self.words.append(word)

	def printTopic(self):
		for word in self.words:
			print word
	
	def getTMLength(self):
		return len(self.words)

	def getSimilarityToParent(self):
		return self.similarityToParent
'''
	Topic Tree container -- holds Topic Models in Hierarchy
'''
class TopicTree:
	'''
		TopicModels argument should be ordered based on hierarchy associated with
		HLDA output
	''' 
	def __init__(self, name_init):
		self.name = name_init
		self.TopicModels = []
	
	def push(self, TopicModel):
		self.TopicModels.append(TopicModel)
		print "this topic model contains " + str(TopicModel.getTMLength()) + " words"

	# def initAdjMatrix(self):
	# 	self.adjMatrix = []
	# 	for topicModel in self.TopicModels: 
	# 		row = [None for x in range(0, topicModel.getLength())]
	# 		self.adjMatrix.append(row)
	# 	print self.adjMatrix

	def getNumTM(self):
		return len(self.TopicModels)

	def calcSimilarityScore(self, topicModel1, wordIdx1, topicModel2, wordIdx2, simType):
		word1 = wn.synsets(topicModel1.words[wordIdx1])
		if (len(word1) == 0): # empty synset object list returned
			return 0
		else: 
			word1 = word1[0] # first synset object from list
		word2 = wn.synsets(topicModel2.words[wordIdx2])
		if (len(word2) == 0): # empty synset object list returned
			return 0 
		else: 
			word2 = word2[0] # first synset object from list

		# return similarity score
		similarityScore = 0
		# determine similarity index
		if (simType == "lch"):
			similarityScore = word1.lch_similarity(word2)
		elif (simType == "wup"):
			similarityScore = word1.wup_similarity(word2)
		else: 
			similarityScore = word1.path_similarity(word2)
		if similarityScore == None: 
			return 0
		else:
			return similarityScore
	#def getDissimilarityIndex()

def main():
	langstonTree = TopicTree('LangstonTree')
	with open('Langston.out') as f:
	    listOfTopics = f.readlines()
	for topic in listOfTopics:
		langstonTree.push(TopicModel(topic.split(' ')))
	# for each child topic model
	for i in range(1, langstonTree.getNumTM()):
		# for each word in the child topic model
		for childWordIdx in range(0, langstonTree.TopicModels[i].getTMLength()):
			# calculate its similarity to each of those in the parent topic model
			for parentWordIdx in range(0, langstonTree.TopicModels[i-1].getTMLength()):
				simScore = langstonTree.calcSimilarityScore(langstonTree.TopicModels[i], childWordIdx, langstonTree.TopicModels[i-1], parentWordIdx, 'default')
				langstonTree.TopicModels[i].similarityToParent += simScore
	for i in range(1, langstonTree.getNumTM()):
		print langstonTree.TopicModels[i].getSimilarityToParent()

main()