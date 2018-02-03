cd ../build
cmake -DCMAKE_BUILD_TYPE=Release ..
make assign_id
cd ../script

dataset="cifar60k"

input_base="/data/jinfeng/project/github/gqr/data/${dataset}/${dataset}_base.fvecs"
output_base="../data/${dataset}/${dataset}_base_with_id.fvecs"
input_query="/data/jinfeng/project/github/gqr/data/${dataset}/${dataset}_query.fvecs"
output_query="../data/${dataset}/${dataset}_query_with_id.fvecs"

../build/bin/assign_id $input_base $output_base
../build/bin/assign_id $input_query $output_query
