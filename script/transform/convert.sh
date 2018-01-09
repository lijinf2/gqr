origin_dataset="audio"
new_dataset="transformed_audio"


mkdir ../../data/${new_dataset}


g++ -I ../../include --std=c++11 -O3 euclidToAngular.cpp -o ./euclidToAngular.bin 

new_query_file="../../data/${new_dataset}/${new_dataset}_query.fvecs"
new_base_file="../../data/${new_dataset}/${new_dataset}_base.fvecs"

origin_query="../../data/${origin_dataset}/${origin_dataset}_query.fvecs"
origin_file="../../data/${origin_dataset}/${origin_dataset}_base.fvecs"

./euclidToAngular.bin ${origin_file} ${new_base_file} ${origin_query} ${new_query_file}
