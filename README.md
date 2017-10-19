GQR
==========
# A General and Efficient Querying Method for Learning to Hash
-----------------------------------------------------------------------------------------------------------------

# Dependences
- CMake
- Matlab

Please use the command "git clone --recursive" to include the submodules

# Run
- cd learn/PCAH 
- mkdir hashingCodeTXT && matlab < pcah.m
- cd ../../ 
- mkdir build && cd ./build && cmake ../ -DCMAKE_BUILD_TYPE=Release
- make search
- cd ../script && sh search.sh

# Parameters
-- hash_method
  options: PCAH - PCA hashing, ITQ - Iterative Quantization
  
-- query_method
  options: 
    GQR - Generate-to-probe Quantization Ranking,
    GHR/HL - Generate-to-probe Hamming Ranking,
    QR - Quantization Ranking,
    HR - Hamming Ranking.
