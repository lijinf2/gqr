#!/usr/bin/env bash
origin_dataset="audio"
new_dataset="e2a_audio"
operation="e2m m2a"

mkdir ../../data/${new_dataset}


g++ -I ../../include --std=c++11 -O3 convert.cpp -o ./convert.bin

new_query_file="../../data/${new_dataset}/${new_dataset}_query.fvecs"
new_base_file="../../data/${new_dataset}/${new_dataset}_base.fvecs"

origin_query="../../data/${origin_dataset}/${origin_dataset}_query.fvecs"
origin_file="../../data/${origin_dataset}/${origin_dataset}_base.fvecs"

./convert.bin ${origin_file} ${new_base_file} ${origin_query} ${new_query_file} ${operation}
