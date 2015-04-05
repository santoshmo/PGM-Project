setwd('~/PGM-Project/SHMP/data/data.processed/')
library(lda)
library(foreach)
# load functions used for ranking documents
source('~/PGM-Project/SHMP/code/tpm_fcn.r')

# read and pre-process corpus for lda modeling
filename_list=list.files()
file_list=foreach(file = filename_list)%do%{readLines(file)}
file_word_num=unlist(lapply(file_list, length))
voca_list=do.call(list,lapply(1:length(file_list), function(x)lexicalize(file_list[[x]])$voca))
voca=unlist(voca_list)
documents_list=lexicalize(file_list)$documents

# run lda model 10 times (using gibbs sampling) to get 10 estimates for rank
# using the multiple imputation combining rule to get estimated rank and corresponding variance
# argument fcn='Laplacian' or 'CV_score'

M=10
mult_result=do.call(list,lapply(1:M,function(x) MI_avg_rank(fcn='Laplacian',file_word_num,documents_list,10,voca,1000)))
avg_mi=rowMeans(do.call(cbind,lapply(1:M,function(x) unlist(mult_result[[x]][1]))))
avg_within_var=rowMeans(do.call(cbind,lapply(1:M,function(x) unlist(mult_result[[x]][2]))))
between_var=do.call(sum,lapply(1:M,function(x) (unlist(mult_result[[x]][1])-avg_mi)^2))/(M-1)
total_var=avg_within_var+((M+1)/M)*between_var
