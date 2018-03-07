mkdir output

dataset="sample.glove.twitter.27B.200d.txt"
g++ -O3 glove_to_fvecs.cpp -o ./output/glove_to_fvecs.bin

./output/glove_to_fvecs.bin $dataset ./output/$dataset.fvecs
