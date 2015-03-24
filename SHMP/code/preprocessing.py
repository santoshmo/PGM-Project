__author__ = 'zhuoyunz'


import nltk
import math
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

def remove_stop(word_list):
    original = word_list
    my_stop = sp.words('english')
    word_list = []
    for w in original:
        if not w.lower() in my_stop and w != None:
            word_list.append(w)
    return word_list

def lemmatization(word_list):
    original = word_list
    word_list = []
    for w in original:
        w = wordnet_lemmatizer.lemmatize(w)
        word_list.append(w)
    return word_list


def word_preprocesing(txt_file):
    wordlist = prep_tokenize(txt_file)
    wordlist = remove_stop(wordlist)
    wordlist = lemmatization(wordlist)
    return wordlist

def get_specific_word(wordlist):
    wordlist = [word for word in wordlist if wn.synsets(word)!= []]
    specific_word = [word for word in wordlist if wn.synsets(word)[0].hypernyms() == []]
    return specific_word

def main():


    fileHandle = open('a0.txt','r')
    Input_text = fileHandle.read()
    word_preprocessed = word_preprocesing(Input_text)
    specific_words = get_specific_word(word_preprocessed)
    write_file = open('a0_processed.txt','w')
    for i in word_preprocessed:
        write_file.write("%s\n" % i)

    write_file = open('a0_specific.txt','w')
    for i in specific_words:
        write_file.write("%s\n" % i)


if __name__ == '__main__':
    main()
