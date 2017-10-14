cd ../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
make cal_groundtruth 2>&1 | tee ../script/log.txt
cd ../script
log=`grep error log.txt`
if [ "$log" != "" ]; then
    exit
fi
mkdir groundtruth

dataset="sift1m"
topk=20
numThreads=20;

ivecs_bench_file="./groundtruth/${dataset}_groundtruth.ivecs"
lshbox_bench_file="./groundtruth/${dataset}_groundtruth.lshbox"

base_file="../data/${dataset}/${dataset}_base.fvecs"
query_file="../data/${dataset}/${dataset}_query.fvecs"

../build/bin/cal_groundtruth $base_file $query_file $topk $lshbox_bench_file $ivecs_bench_file $numThreads
