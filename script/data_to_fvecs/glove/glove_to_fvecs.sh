mkdir output

g++ -O3 glove_to_fvecs.cpp -o ./output/glove_to_fvecs.bin

./output/glove_to_fvecs.bin sample.glove.twitter.27B.200d.txt ./output/sample.glove1.2m_base.fvecs
