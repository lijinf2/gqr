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

hash_method="SIM"
query_method="HR"
num_tables=1
codelength=64
TYPE_DIST="AG"
base_format="fvecs"

# dataset="m2a_movielens"
# cardinality=69876
# dimension=102
# num_queries=10677

dataset="m2a_netflix"
cardinality=17770
dimension=302
num_queries=60000

topk=20

model_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}model${dataset^^}${codelength}b_${num_tables}tb.txt"
base_file="../data/${dataset}/${dataset}_base.fvecs"
base_bits_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}table${dataset^^}${codelength}b_${num_tables}tb.txt"
query_file="../data/${dataset}/${dataset}_query.fvecs"
benchmark_file="../data/${dataset}/${dataset}_angular_groundtruth.lshbox"

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
