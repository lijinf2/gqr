numRecords=1000
dimension=128
mean=0
stdDev=10 # standard deviation
fileName="random_${numRecords}_${dimension}_${mean}_${stdDev}.fvecs"
echo $fileName
mkdir output

g++ --std=c++11 -O3 random.cpp -o ./output/random.bin

./output/random.bin output/${fileName} ${numRecords} ${dimension} ${mean} ${stdDev}
