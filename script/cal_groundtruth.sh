cd ../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
make cal_groundtruth 2>&1 | tee ../script/log.txt
cd ../script
log=`grep error log.txt`
if [ "$log" != "" ]; then
    exit
fi

topk=20
numThreads=8;

iter=0
# for dataset in "audio" "gist" "sift1m" "glove2.2m" "tiny5m" "deep1M" "sift10m"
for dataset in "cifar60k"
do
    iter=`expr $iter + 1`

    base_file="../data/${dataset}/${dataset}_base.fvecs"
    query_file="../data/${dataset}/${dataset}_query.fvecs"

    ivecs_bench_file="../data/${dataset}_groundtruth.ivecs"
    lshbox_bench_file="../data/${dataset}_groundtruth.lshbox"

    ../build/bin/cal_groundtruth $base_file $query_file $topk $lshbox_bench_file $ivecs_bench_file $numThreads
done
