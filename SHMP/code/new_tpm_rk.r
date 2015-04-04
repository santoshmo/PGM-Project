# document ranking based on re-rank topics given by LDA

setwd('~/PGM-Project/SHMP/data/data.processed/')

library(lda)
library(foreach)
filename_list=list.files()
file_list=foreach(file = filename_list)%do%{readLines(file)}
file_word_num=unlist(lapply(file_list, length))

# topic modeling using lda
voca_list=do.call(list,lapply(1:length(file_list), function(x)lexicalize(file_list[[x]])$voca))
voca=unlist(voca_list)
documents_list=lexicalize(file_list)$documents
# lda gives 10 topics, iterates for 1000 times, alpha=beta=0.1
tpc.model = lda.collapsed.gibbs.sampler(documents_list, 10, voca, 1000, 0.1,0.1)

# 1. Weighted Topic Coverage and Variation
documents_dist=apply(tpc.model$document_sums,2,function(x) x/sum(x))
mu_zk=documents_dist%*%file_word_num/sum(file_word_num)
sigma_zk=sqrt((apply(documents_dist,2,function(x) x-mu_zk))^2%*%file_word_num/sum(file_word_num))

# for cv_tr parameters lambda1 and lambda2 should be trained
lambda1=0.5
lambda2=0.5
CV_TR=unlist(do.call(list,lapply(1:length(mu_zk),function(x) mu_zk[x]*sigma_zk[x])))

# in experiment, we use KR1 for keyword reranking
KR1=apply(tpc.model$topics,2,function(x)x/sum(x))

# get top words in most important topic given by the topic-reranking result
tpc.kw.rank=top.topic.words(KR1,10)[,sort(CV_TR,decreasing=TRUE,index.return=TRUE)$ix[1]]
# get document rank based on most important topic given by the topic-reranking result
tpc.document.rank=top.topic.documents(tpc.model$document_sums,5)[,sort(CV_TR,decreasing=TRUE,index.return=TRUE)$ix[1]]


