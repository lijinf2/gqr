origin_dataset="audio"
new_dataset="transformed_audio"
num_sample=1000

mkdir ../../data/${new_dataset}


g++ -I ../../include --std=c++11 -O3 transform.cpp -o ./transform.bin 

new_query_file="../../data/${new_dataset}/${new_dataset}_query.fvecs"
new_base_file="../../data/${new_dataset}/${new_dataset}_base.fvecs"

origin_file="../../data/${origin_dataset}/${origin_dataset}_base.fvecs"


./transform.bin ${origin_file} ${new_base_file} ${new_query_file} ${num_sample}