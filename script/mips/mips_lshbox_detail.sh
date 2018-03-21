#!/bin/bash
cd ../../build 
cmake ../ -DCMAKE_BUILD_TYPE=Debug
make mips_lshbox_detail 2>&1 | tee ../script/log.txt

cd ../script/mips

dataset="movielens"
outputFile="./output_mips_lshbox_detail.txt"
base_file="../../data/${dataset}/${dataset}_base.fvecs"
query_file="../../data/${dataset}/${dataset}_query.fvecs"
benchmark_file="../../data/${dataset}/${dataset}_product_groundtruth.lshbox"
gdb --args ../../build/bin/mips_lshbox_detail $base_file $query_file $benchmark_file $outputFile
