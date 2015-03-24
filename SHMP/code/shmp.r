setwd('~/SHMP/data/')
#install.packages('lda',dependencies=TRUE)
library(lda)
txt=readLines('a4_processed.txt')
documents = lexicalize(txt, lower=TRUE)
voca = documents$vocab
result = lda.collapsed.gibbs.sampler(documents$documents, 10, voca, 1000, 0.1,0.1)
num=max(result$topic_sums)
idx=which.max(result$topic_sums)
D=top.topic.words(result$topics,num)[,idx]
specific_word = readLines('a4_specific.txt')
score = length(D[(D%in%specific_word)])/length(D)
score
