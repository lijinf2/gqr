cd ../../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
cd ../learn/KNNGraph

topk=20
numThreads=20;

# metric="euclidean"
# metric="angular"
metric="product"

iter=0
# for dataset in "audio" "gist" "sift1m" "glove2.2m" "tiny5m" "deep1M" "sift10m"
for dataset in "audio"
do
    iter=`expr $iter + 1`

    # input data files
    base_file="../../data/${dataset}/${dataset}_base.fvecs"
    query_file="../../data/${dataset}/${dataset}_base.fvecs"

    # output groundtruth files, support both ivecs and lshbox formats 
    ivecs_bench_file="${dataset}_${metric}_groundtruth.ivecs"
    lshbox_bench_file="${dataset}_${metric}_groundtruth.lshbox"

    echo '[cal KNNs] start calculate K-Nearest Neighbor'
    ../../build/bin/cal_groundtruth $base_file $query_file $topk $lshbox_bench_file $ivecs_bench_file $metric $numThreads

    echo '[cal KNNs] start transform K-Nearest Neighbor into module format'
    python knngraph.py $topk $dataset $metric
    echo '[cal KNNs] remove temporary files'
    rm $ivecs_bench_file
    rm $lshbox_bench_file
done

