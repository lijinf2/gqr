# parameters
topk=20
numThreads=20;

# shell scripts
metric="euclidean"
mkdir ../build
cd ../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
make cal_groundtruth 2>&1 | tee ../reproduce/log.txt
make search 2>&1 | tee ../reproduce/log.txt
cd ../reproduce

# for dataset in "cifar60k" "gist" 
for dataset in "audio" "cifar60k" 
do

# get dataset
name="${dataset}.tar.gz"
wget http://www.cse.cuhk.edu.hk/systems/hash/gqr/dataset/${name} -P ../data/
tar zxvf ../data/${name} -C ../data/

# calculate groundtruth
base_file="../data/${dataset}/${dataset}_base.fvecs"
query_file="../data/${dataset}/${dataset}_query.fvecs"

# output groundtruth files, support both ivecs and lshbox formats 
ivecs_bench_file="../data/${dataset}/${dataset}_groundtruth.ivecs"
lshbox_bench_file="../data/${dataset}/${dataset}_groundtruth.lshbox"
if [ $metric != "euclidean" ]
then
    ivecs_bench_file="../data/${dataset}/${dataset}_${metric}_groundtruth.ivecs"
    lshbox_bench_file="../data/${dataset}/${dataset}_${metric}_groundtruth.lshbox"
fi

../build/bin/cal_groundtruth $base_file $query_file $topk $lshbox_bench_file $ivecs_bench_file $metric $numThreads
done
