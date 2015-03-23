__author__ = 'kevin'
#This is the definition of a function that gives a score to a txt file
import nltk
import math
from tag_conversion import *
from nltk.corpus import wordnet as wn
from nltk.corpus import stopwords as sp
from nltk.corpus import words as wd
from nltk.tokenize import word_tokenize
from nltk.tokenize import RegexpTokenizer
from nltk.stem import WordNetLemmatizer
#definition
wordnet_lemmatizer = WordNetLemmatizer()

def prep_tokenize(txt_file):
    toker = RegexpTokenizer(r'((?<=[^\w\s])\w(?=[^\w\s])|(\W))+', gaps=True)
    word_list = toker.tokenize(txt_file)
    return word_list

def tag_words(word_list):
    tag_text = nltk.pos_tag(word_list)
    word_list = []
    for (string,tag) in tag_text:
        word_list.append([string, penn_to_wn(tag), tag])
    return word_list

def remove_stop(word_list):
    original = word_list
    my_stop = sp.words('english')
    word_list = []
    for w in original:
        if not w[0].lower() in my_stop and w[1] != None:
            word_list.append(w)
    return word_list

def lemmatization(word_list):
    original = word_list
    word_list = []
    for w in original:
        w[0] = wordnet_lemmatizer.lemmatize(w[0], pos = w[1])
        word_list.append(w)
    return word_list

def remove_duplication(word_list):
    original = word_list
    word_list = []
    for w in original:
        if w not in word_list:
            word_list.append(w)
    return word_list

def create_s(word_list):
    synset_list = []
    for w in word_list:
        if w[1] == wn.NOUN:
            ws = wn.synsets(w[0],pos = w[1])
            if ws:
                synset_list.append(ws[0])
    return synset_list

def scope_score(synset_list):
    scope = 0
    for w in synset_list:
        root_synset = w.root_hypernyms()
        root_synset = root_synset[0]
        depth = w.shortest_path_distance(root_synset)
        scope = scope + depth
    scope = scope/(len(synset_list))
    scope = math.exp(-scope)
    return scope

def similarity_score(synset_list):
    size = len(synset_list)
    sum_similarity = 0
    for x in xrange(0,size):
        for y in xrange(x+1,size):
            sum_similarity = sum_similarity + wn.path_similarity(synset_list[x],synset_list[y])
    d_similarity = sum_similarity/(size*(size-1)/2)
    return d_similarity


def doc_generality(txt_file):
    wordlist = prep_tokenize(txt_file)
    wordlist = tag_words(wordlist)
    wordlist = remove_stop(wordlist)
    wordlist = lemmatization(wordlist)
    wordlist = remove_duplication(wordlist)
    synsetlist = create_s(wordlist)

    #calculate the score
    scope = scope_score(synsetlist)
    similarity = similarity_score(synsetlist)
    score = scope/(similarity+1)
    return score