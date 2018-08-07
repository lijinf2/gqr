hash_method=$1
query_method=$2
codelength="${3}"
num_tables="${4}"
dataset=$5
base_format="fvecs"

# trian model
cd ../learn/${hash_method}
matlab -r "${hash_method}('${dataset}', ${codelength}, ${num_tables});exit"
cd ../../reproduce
# sh runExperiment.sh $hash_method $query_method "${codelength}" "${num_tables}" $dataset $base_format

# run queries
codelength="${codelength}b"
num_tables="${num_tables}tb"

dataset_name_upper=$(tr '[a-z]' '[A-Z]' <<< $dataset)
model_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}model${dataset_name_upper}${codelength}_${num_tables}.txt"
base_file="../data/${dataset}/${dataset}_base.fvecs"
base_bits_file="../learn/${hash_method}/hashingCodeTXT/${hash_method}table${dataset_name_upper}${codelength}_${num_tables}.txt"
query_file="../data/${dataset}/${dataset}_query.fvecs"
benchmark_file="../data/${dataset}/${dataset}_groundtruth.lshbox"

../build/bin/search\
    --hash_method=$hash_method \
    --query_method=$query_method \
    --base_format=$base_format \
    --model_file=$model_file\
    --base_file=$base_file \
    --base_bits_file=$base_bits_file \
    --query_file=$query_file \
    --benchmark_file=$benchmark_file
