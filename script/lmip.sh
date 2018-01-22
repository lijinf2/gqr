#!/bin/bash
cd ../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
make search 2>&1 | tee ../script/log.txt
cd ../script
log=`grep error log.txt`
if [ "$log" != "" ]; then
    exit
fi

hash_method="LMIP"
query_method="LM"
num_tables=1
codelength=54
TYPE_DIST="IP"

dataset="audio"
base_format="fvecs"
cardinality=53387
dimension=192
num_queries=200
topk=20

#msong 
# dataset="msong"
# base_format="fvecs"
# cardinality=994185
# dimension=420
# num_queries=1000
# topk=20

# sift1m
# dataset="sift1m"
# base_format="fvecs"
# cardinality=1000000
# dimension=128
# num_queries=1000
# topk=20



model_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}model${dataset^^}${codelength}b_${num_tables}tb.txt"
base_file="../data/${dataset}/${dataset}_base.fvecs"
base_bits_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}table${dataset^^}${codelength}b_${num_tables}tb.txt"
query_file="../data/${dataset}/${dataset}_query.fvecs"
benchmark_file="../data/${dataset}/${dataset}_product_groundtruth.lshbox"

# gdb --args \
 ../build/bin/search\
    --hash_method=$hash_method \
    --query_method=$query_method \
    --base_format=$base_format \
    --cardinality=$cardinality \
    --dimension=$dimension \
    --num_queries=$num_queries \
    --topk=$topk \
    --num_tables=$num_tables \
    --model_file=$model_file\
    --base_file=$base_file \
    --base_bits_file=$base_bits_file \
    --query_file=$query_file \
    --benchmark_file=$benchmark_file \
    --TYPE_DIST=$TYPE_DIST
