origin_dataset="audio"



g++ -I ../../include --std=c++11 -O3 fvecs_to_txt.cpp -o ./fvecs_to_txt.bin 


input_file="../../data/${origin_dataset}/${origin_dataset}_base.fvecs"
output_file="${origin_dataset}.txt"


./fvecs_to_txt.bin ${input_file} ${output_file}