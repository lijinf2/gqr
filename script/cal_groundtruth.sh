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

dataset="glove2.2m"
topk=1000
numThreads=20;

benchmark_file="./groundtruth/${dataset}_groundtruth.ivecs"
lshbox_ben_file="./groundtruth/${dataset}_groundtruth.lshbox"

base_file="../data/${dataset}/${dataset}_base.fvecs"
query_file="../data/${dataset}/${dataset}_query.fvecs"

../build/bin/cal_groundtruth $base_file $query_file $topk $benchmark_file $numThreads > $lshbox_ben_file
