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

wgt_tpc=function(word_num,dc_list,tpc_num,vocab,iter_num,alpha,beta,lambda1,lambda2){
      
      lda.model=lda.collapsed.gibbs.sampler(dc_list,tpc_num,vocab,iter_num,alpha,beta)
      dc_sums=lda.model$document_sums
      dc_dist=apply(dc_sums,2,function(x) x/sum(x))
      mu_zk=documents_dist%*%word_num/sum(word_num)
      sigma_zk=(apply(dc_dist,2,function(x) x-mu_zk))^2%*%word_num/sum(word_num)
      cv_tr=apply(cbind(mu_zk,sigma_zk),1,function(x) x=(x[1]^lambda1)*(x[2]^lambda2))
      rank=top.topic.documents(lda.model$document_sums,5)[,sort(cv_tr,decreasing=TRUE,index.return=TRUE)$ix[1:5]]
      wgt_rank=rank%*%seq(5,1)/sum(seq(5,1))
      wgt_rank_var=(sum(seq(5,1)^2)/(sum(seq(5,1))^2))*(rowMeans(apply(rank,2,function(x) (x-rowMeans(rank))^2)))
      return(list(wgt_rank,wgt_rank_var))
}

# 1. Weighted Topic Coverage and Variation
documents_dist=apply(tpc.model$document_sums,2,function(x) x/sum(x))
mu_zk=documents_dist%*%file_word_num/sum(file_word_num)
sigma_zk_between=sqrt((apply(documents_dist,2,function(x) x-mu_zk))^2%*%file_word_num/sum(file_word_num))

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

# 1.1 multiple imputation for weighted topic coverage and variation
# run lda model 10 times (using gibbs sampling) to get 10 estimates for rank
# using the multiple imputation combining rule to get estimated rank and corresponding variance
# which propagte uncertainty of simulation

M=10

mult_result=do.call(list,lapply(1:M,function(x) wgt_tpc(file_word_num,documents_list,10,voca,1000,0.1,0.1,0.5,0.5)))
avg_coverage=rowMeans(do.call(cbind,lapply(1:M,function(x) unlist(mult_result[[x]][1]))))
avg_within_var=rowMeans(do.call(cbind,lapply(1:M,function(x) unlist(mult_result[[x]][2]))))
between_var=do.call(sum,lapply(1:M,function(x) (unlist(mult_result[[x]][1])-avg_coverage)^2))/(M-1)
total_var=avg_within_var+((M+1)/M)*between_var



