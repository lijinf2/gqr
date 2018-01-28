cd ../build 
cmake ../ -DCMAKE_BUILD_TYPE=Debug
# cmake ../ -DCMAKE_BUILD_TYPE=Release
make sample_remove_queries 2>&1 | tee ../script/log.txt
cd ../script
log=`grep error log.txt`
if [ "$log" != "" ]; then
    exit
fi


dataset="audio"
num_queries=1000
base_file="../data/${dataset}/${dataset}_base.fvecs"
base_output_file="./${dataset}_base.fvecs"
query_output_file="./${dataset}_query.fvecs"

gdb --args ../build/bin/sample_remove_queries $base_file $num_queries $query_output_file $base_output_file
