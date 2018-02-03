cd ../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
make cal_recall 2>&1 | tee ../script/log.txt
cd ../script
#log=`grep error log.txt`
#if [ "$log" != "" ]; then
#    exit
#fi

dataset="cifar60k"
part_num=20
app_output="./output/${dataset}/part-"
groundtruth="../data/${dataset}/${dataset}_groundtruth.lshbox"
output_file="${dataset}_recall.txt"

rm ./output/${dataset}/*
hadoop fs -get /ysong/${dataset}/part* ./output/${dataset}/

../build/bin/cal_recall $part_num $app_output $groundtruth $output_file 
