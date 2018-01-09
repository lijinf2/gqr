cd ../build 
cmake ../ -DCMAKE_BUILD_TYPE=Debug
# cmake ../ -DCMAKE_BUILD_TYPE=Release
make bin_to_fvecs 2>&1 | tee ../script/log.txt
cd ../script
log=`grep error log.txt`
if [ "$log" != "" ]; then
    exit
fi
mkdir fvecs


dataset="cifar60k"
dimension=512
bin_file="../data/${dataset}/${dataset}.bin"
fvecs_file="./fvecs/${dataset}.fvecs"

gdb --args ../build/bin/bin_to_fvecs $bin_file $fvecs_file $dimension
