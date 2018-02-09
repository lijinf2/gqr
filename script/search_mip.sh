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
## mips

# lmip
# hash_method="LMIP"
# query_method="LM"
# codelength="12b" # 12b 27b 58b
# num_tables="1tb"
# metric="product"

# # nlmip
# hash_method="NLMIP"
# query_method="NR"
# # query_method="IMIP"
# codelength="25b_128i" # 12b 27b 58b
# num_tables="1tb"
# metric="product"
# #
# # # dataset="movielens"
# dataset="netflix"

# simpleLSH
# hash_method="SIM"
# query_method="HR"
# codelength="64b" # 16b 32b 64b
# num_tables="1tb"
# metric="angular"
# dataset="m2a_movielens"
# # dataset="m2a_netflix"

# # # ALSH
# hash_method="IntRankALSH"
hash_method="ALSHRank"
codelength="32b" # 16b 32b 64b
num_tables="1tb"
metric="product"
# dataset="movielens"
dataset="netflix"

# # E2LSH
# hash_method="E2LSH"
# query_method="IntRank"
# codelength="16b" # 16b 32b 64b
# num_tables="1tb"
# metric="product"
# # # # dataset="movielens"
# dataset="netflix"

base_format="fvecs"

model_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}model${dataset^^}${codelength}_${num_tables}.txt"
base_file="../data/${dataset}/${dataset}_base.fvecs"
base_bits_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}table${dataset^^}${codelength}_${num_tables}.txt"
query_file="../data/${dataset}/${dataset}_query.fvecs"

benchmark_file="../data/${dataset}/${dataset}_groundtruth.lshbox"
if [ $metric != "euclidean" ]
then
    benchmark_file="../data/${dataset}/${dataset}_${metric}_groundtruth.lshbox"
fi

../build/bin/search\
    --hash_method=$hash_method \
    --query_method=$query_method \
    --base_format=$base_format \
    --model_file=$model_file\
    --base_file=$base_file \
    --base_bits_file=$base_bits_file \
    --query_file=$query_file \
    --benchmark_file=$benchmark_file \
    --metric=$metric

