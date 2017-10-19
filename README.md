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
