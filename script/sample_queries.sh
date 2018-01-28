cd ../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
make sample_queries 2>&1 | tee ../script/log.txt
cd ../script
log=`grep error log.txt`
if [ "$log" != "" ]; then
    exit
fi


dataset="movielens"
num_queries=200
# base_file="../data/${dataset}/${dataset}_base.fvecs"
# query_file="../data/${dataset}/${dataset}_query.fvecs"

query_file="../data/${dataset}/${dataset}_query.fvecs.new"
base_file="../data/${dataset}/${dataset}_query.fvecs"

../build/bin/sample_queries $base_file $num_queries $query_file
