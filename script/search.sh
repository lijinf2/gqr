#!/bin/bash
mkdir ../build 
cd ../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
make search 2>&1 | tee ../script/log.txt
cd ../script
log=`grep error log.txt`
if [ "$log" != "" ]; then
    exit
fi

### search methods

## knngraph
# hash_method="KNNGraph"
# query_method="KGS"
# codelength=""
# num_tables="20k"

## e2lsh
# hash_method="E2LSH"
# query_method="IntRank"
# codelength="12b"
# num_tables="1tb"

## binary hashing
hash_method="PCAH"
query_method="GQR"
codelength="12b"
num_tables="1tb"

### datasets
# audio
dataset="audio"
base_format="fvecs"
cardinality=53387
dimension=192

#msong 
# dataset="msong"
# base_format="fvecs"
# cardinality=994185
# dimension=420

#glove1.2m
# dataset="glove1.2m"
# base_format="fvecs"
# cardinality=1193514
# dimension=200

# #imagenet
# dataset="imagenet"
# base_format="fvecs"
# cardinality=2340373
# dimension=150

#nuswide
# dataset="nuswide"
# base_format="fvecs"
# cardinality=268643
# dimension=500

#ukbench
# dataset="ukbench"
# base_format="fvecs"
# cardinality=1097907
# dimension=128

#enron
# dataset="enron"
# base_format="fvecs"
# cardinality=94987
# dimension=1369

#cifar60k
# dataset="cifar60k"
# base_format="fvecs"
# cardinality=60000
# dimension=512

# # gist
# dataset="gist"
# base_format="fvecs"
# cardinality=1000000
# dimension=960

#sift1m
# dataset="sift1m"
# base_format="fvecs"
# cardinality=1000000
# dimension=128


# #tiny5m
# dataset="tiny5m"
# base_format="fvecs"
# cardinality=5000000
# dimension=384

#sift10m
# dataset="sift10m"
# base_format="fvecs"
# cardinality=10000000
# dimension=128

# #glove2.2m
# dataset="glove2.2m"
# base_format="fvecs"
# cardinality=2196017
# dimension=300
#
# deep1m
# dataset="deep1M"
# base_format="fvecs"
# cardinality=1000000
# dimension=256

model_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}model${dataset^^}${codelength}_${num_tables}.txt"
base_file="../data/${dataset}/${dataset}_base.fvecs"
base_bits_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}table${dataset^^}${codelength}_${num_tables}.txt"
query_file="../data/${dataset}/${dataset}_query.fvecs"
benchmark_file="../data/${dataset}/${dataset}_groundtruth.lshbox"

../build/bin/search\
    --hash_method=$hash_method \
    --query_method=$query_method \
    --base_format=$base_format \
    --cardinality=$cardinality \
    --dimension=$dimension \
    --model_file=$model_file\
    --base_file=$base_file \
    --base_bits_file=$base_bits_file \
    --query_file=$query_file \
    --benchmark_file=$benchmark_file
