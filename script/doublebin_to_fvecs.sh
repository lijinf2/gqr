cd ../build 
cmake ../ -DCMAKE_BUILD_TYPE=Debug
# cmake ../ -DCMAKE_BUILD_TYPE=Release
make doublebin_to_fvecs 2>&1 | tee ../script/log.txt
cd ../script
log=`grep error log.txt`
if [ "$log" != "" ]; then
    exit
fi
mkdir fvecs


dataset="imagenet"
dimension=150

query_doublebin_file="../data/${dataset}/${dataset}_query.bin"
query_fvecs_file="./fvecs/${dataset}_query.fvecs"

base_doublebin_file="../data/${dataset}/${dataset}_base.bin"
base_fvecs_file="./fvecs/${dataset}_base.fvecs"

../build/bin/doublebin_to_fvecs $query_doublebin_file $query_fvecs_file $dimension
../build/bin/doublebin_to_fvecs $base_doublebin_file $base_fvecs_file $dimension
